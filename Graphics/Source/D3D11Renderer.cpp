#include "D3D11Renderer.h"

namespace graphics
{
	namespace
	{
		void CreateDockingWindow()
		{
			static bool show_app_fullscreen = false;
			bool* p_open = &show_app_fullscreen;

			static bool opt_fullscreen = true;
			static bool opt_padding = false;
			static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
			// because it would be confusing to have two docking targets within each others.
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			if (opt_fullscreen)
			{
				const ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->WorkPos);
				ImGui::SetNextWindowSize(viewport->WorkSize);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			}
			else
			{
				dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
			}

			// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
			// and handle the pass-thru hole, so we ask Begin() to not render a background.
			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
				window_flags |= ImGuiWindowFlags_NoBackground;

			// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
			// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
			// all active windows docked into it will lose their parent and become undocked.
			// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
			// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
			if (!opt_padding)
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace Demo", p_open, window_flags);
			if (!opt_padding)
				ImGui::PopStyleVar();

			if (opt_fullscreen)
				ImGui::PopStyleVar(2);

			// Submit the DockSpace
			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Options"))
				{
					// Disabling fullscreen would allow the window to be moved to the front of other windows,
					// which we can't undo at the moment without finer window depth/z control.
					ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
					ImGui::MenuItem("Padding", NULL, &opt_padding);
					ImGui::Separator();

					if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
					if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
					if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
					if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
					if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
					ImGui::Separator();

					if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
						*p_open = false;
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			ImGui::End();
		}

		void DrawVector3Control(const std::string& label, XMFLOAT3& values, float resetValue = 0.f, float columnWidth = 100.f)
		{
			ImGui::PushID(label.c_str());

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGui::Text(label.c_str());
			ImGui::NextColumn();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5,0 });
			
			float width = ImGui::GetColumnWidth() - columnWidth;

			float lineHeight = ImGui::GetFrameHeight();
			ImVec2 buttonSize = { lineHeight + 3.f, lineHeight };
			ImGui::PushItemWidth(width/3);
			if (ImGui::Button("X", buttonSize))
				values.x = resetValue;
			ImGui::SameLine();
			ImGui::DragFloat("##X", &values.x, 0.f, -1.f, 1.f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushItemWidth(width /3);
			if (ImGui::Button("Y", buttonSize))
				values.y = resetValue;
			ImGui::SameLine();
			ImGui::DragFloat("##Y", &values.y, 0.f, -1.f, 1.f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushItemWidth(width / 3);
			if (ImGui::Button("Z", buttonSize))
				values.z = resetValue;
			ImGui::SameLine();
			ImGui::DragFloat("##Z", &values.z, 0.f, -1.f, 1.f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PopStyleVar(); 

			ImGui::Columns(1);

			ImGui::PopID();
		}

	}// anonymouse namespace 
	void D3D11Renderer::BuildFilters() {

		m_postProcesses.clear();

		std::shared_ptr<D3D11PostProcess> copyFilter =
			std::make_shared<D3D11PostProcess>(m_d3dDevice, m_d3dContext, L"Sampling", L"Sampling",
				m_screenWidth, m_screenHeight);
		copyFilter->SetShaderResources({this->m_shaderResourceView});
		m_postProcesses.push_back(copyFilter);

		std::shared_ptr<D3D11PostProcess> blurFilter =
			std::make_shared<D3D11PostProcess>(m_d3dDevice, m_d3dContext, L"Sampling", L"Sampling",
				m_screenWidth, m_screenHeight);
		blurFilter->SetShaderResources({ this->m_shaderResourceView });
		blurFilter->m_pixelConstData.threshold = 0.f;
		blurFilter->UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
		m_postProcesses.push_back(blurFilter);
		for (int down = m_down; down >= 1; down /= 2)
		{
			for (int i = 0; i < 5; i++) 
			{
				auto& prevResource = m_postProcesses.back()->m_shaderResourceView;
				m_postProcesses.push_back(std::make_shared<D3D11PostProcess>(
					m_d3dDevice, m_d3dContext, L"Sampling", L"Blur",
					m_screenWidth / m_down, m_screenHeight / m_down));
				m_postProcesses.back()->SetShaderResources({ prevResource });
			}
			if (down > 1) 
			{
				auto upFilter = std::make_shared<D3D11PostProcess>(
					m_d3dDevice, m_d3dContext, L"Sampling", L"Sampling",
					m_screenWidth / down * 2, m_screenHeight / down * 2);
				upFilter->SetShaderResources({ m_postProcesses.back()->m_shaderResourceView });
				upFilter->m_pixelConstData.threshold = 0.0f;
				upFilter->UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
				m_postProcesses.push_back(upFilter);
			}
		}

		auto combineFilter =
			std::make_shared<D3D11PostProcess>(m_d3dDevice, m_d3dContext, L"Sampling", L"Combine",
				m_screenWidth, m_screenHeight);
		combineFilter->SetShaderResources({ copyFilter->m_shaderResourceView,
										   m_postProcesses.back()->m_shaderResourceView});
		combineFilter->SetRenderTargets({ this->m_renderTargetView });
		combineFilter->m_pixelConstData.strength = m_strength;
		combineFilter->UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
		m_postProcesses.push_back(combineFilter);
	}

	D3D11Renderer::D3D11Renderer() : D3D11Core(){}


	bool D3D11Renderer::Initialize()
	{
	
		// Initialize Essentials
		if (!D3D11Core::Initialize())
			return false;

		if(!m_cubeMap.Initialize(
			m_d3dDevice,
			L"d:/Atrium_diffuseIBL.dds",
			L"d:/Atrium_specularIBL.dds"))
			return false;

		//if (!my_Mesh1.Intialize(m_d3dDevice, "c:/zelda/zeldaPosed001.fbx"))
		//	return false;

		std::vector<MeshData> meshes;
		meshes.push_back(Geometry::MakeSphere(1.f, 20, 20));
		meshes[0].textureFilename = "d:/earth.jpg";

		std::vector<MeshData> meshes2;
		meshes2.push_back(Geometry::MakeCube(1.f,1.f,1.f));
		meshes2[0].textureFilename = "d:/earth.jpg";

		if (!my_Mesh1.Intialize(m_d3dDevice, meshes))
			return false;

		my_Mesh1.diffuseSRV = m_cubeMap.diffuseSRV;
		my_Mesh1.specularSRV= m_cubeMap.specularSRV;

		if (!my_Mesh2.Intialize(m_d3dDevice, meshes2))
			return false;

		//// 노멀 벡터 그리기
		//// 문제를 단순화하기 위해 InputLayout은 BasicVertexShader와 같이 사용합시다.
		//m_normalLines = std::make_shared<Mesh>();
		//// Mesh *m_normalLines = new Mesh();

		//std::vector<Vertex> normalVertices;
		//std::vector<uint32_t> normalIndices;
		//size_t offset = 0;
		//for (const auto& meshData : meshes) {
		//	for (size_t i = 0; i < meshData.vertices.size(); i++) {

		//		auto v = meshData.vertices[i];

		//		v.texcoord.x = 0.0f; // 시작점 표시
		//		normalVertices.push_back(v);

		//		v.texcoord.x = 1.0f; // 끝점 표시
		//		normalVertices.push_back(v);

		//		normalIndices.push_back(uint32_t(2 * (i + offset)));
		//		normalIndices.push_back(uint32_t(2 * (i + offset) + 1));
		//	}
		//	offset += meshData.vertices.size();
		//}

		//CreateVertexBuffer(m_d3dDevice, normalVertices, m_normalLines->vertexBuffer);
		//m_normalLines->m_indexCount = UINT(normalIndices.size());
		//D3D11Utilities::CreateIndexBuffer(m_d3dDevice, normalIndices, m_normalLines->indexBuffer);
		//CreateConstantBuffer(m_d3dDevice, m_normalVertexConstantBufferData,
		//	m_normalLines->vertexConstantBuffer);

		//D3D11Utilities::CreateVertexShaderAndInputLayout(m_d3dDevice,
		//	L"NormalVertexShader.hlsl", inputElements, m_normalVertexShader,
		//	m_colorInputLayout);
		//D3D11Utilities::CreatePixelShader(m_d3dDevice, L"NormalPixelShader.hlsl", m_normalPixelShader);
		BuildFilters();

		//Initialize Success
		return true;
	}


	void D3D11Renderer::Update(float dt)
	{
		using namespace DirectX;

		//ImGui::Checkbox("Use Rim", reinterpret_cast<bool*>(&m_basicPixe
		//auto basicVertexData{ m_basicVertexConstantBufferData };

		if (m_useFPV)
		{
			if (m_keyPressed[87])
				m_camera.MoveForward(dt);
			if (m_keyPressed[83])
				m_camera.MoveForward(-dt);
			if (m_keyPressed[68])
				m_camera.MoveRight(dt);
			if (m_keyPressed[65])
				m_camera.MoveRight(-dt);
		}

		
		// 모델의 변환
		my_Mesh1.m_basicVertexConstantBufferData.world = XMMatrixScaling(m_modelScaling.x, m_modelScaling.y, m_modelScaling.z) *
			XMMatrixRotationY(m_modelRotation.y*XM_PI) *
			XMMatrixRotationX(m_modelRotation.x * XM_PI) *
			XMMatrixRotationZ(m_modelRotation.z * XM_PI) *
			XMMatrixTranslation(m_modelTranslation.x, m_modelTranslation.y, m_modelTranslation.z);
		my_Mesh1.m_basicVertexConstantBufferData.world = XMMatrixTranspose(my_Mesh1.m_basicVertexConstantBufferData.world);

		// 역행렬의 전치 행렬
		my_Mesh1.m_basicVertexConstantBufferData.invTranspose = my_Mesh1.m_basicVertexConstantBufferData.world;
		my_Mesh1.m_basicVertexConstantBufferData.invTranspose *= XMMatrixTranslation(m_modelTranslation.x, m_modelTranslation.y, m_modelTranslation.z);
		my_Mesh1.m_basicVertexConstantBufferData.invTranspose =
			XMMatrixInverse(nullptr, XMMatrixTranspose(my_Mesh1.m_basicVertexConstantBufferData.invTranspose));

		XMMATRIX viewRow = m_camera.GetViewRow();
		XMMATRIX projRow = m_camera.GetProjRow();
		XMFLOAT3 eyeWorld = m_camera.GetEyePos();
		// 시점 변환

		my_Mesh1.m_basicPixelConstantBufferData.eyeWorld = eyeWorld;
		my_Mesh1.m_basicVertexConstantBufferData.view = XMMatrixTranspose(viewRow);
		my_Mesh1.m_basicVertexConstantBufferData.projection =
			XMMatrixTranspose(projRow);
		
		my_Mesh2.m_basicPixelConstantBufferData.eyeWorld = eyeWorld;
		my_Mesh2.m_basicVertexConstantBufferData.view = XMMatrixTranspose(viewRow);
		my_Mesh2.m_basicVertexConstantBufferData.projection =
			XMMatrixTranspose(projRow);


		if (m_pickColor[0] == 255) {
			my_Mesh1.m_basicPixelConstantBufferData.material.diffuse =
				XMFLOAT3(1.0f, 0.1f, 0.1f);
		}

		/*my_Mesh1.m_basicVertexConstantBufferData.view =
			XMMatrixRotationX(m_viewRot.x) *
			XMMatrixRotationY(m_viewRot.y) *
			XMMatrixRotationZ(m_viewRot.z) *
			XMMatrixTranslation(0.0f, 0.0f, 2.0f);


		XMStoreFloat3(&my_Mesh1.m_basicPixelConstantBufferData.eyeWorld, XMVector3Transform(
			XMVectorZero(), XMMatrixInverse(nullptr, my_Mesh1.m_basicVertexConstantBufferData.view)));

		my_Mesh1.m_basicVertexConstantBufferData.view =
			XMMatrixTranspose(my_Mesh1.m_basicVertexConstantBufferData.view);*/

		//// 프로젝션
		//m_aspect = D3D11Core::GetAspectRatio();
		//if (m_usePerspectiveProjection) {
		//	my_Mesh1.m_basicVertexConstantBufferData.projection =
		//		XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY), m_aspect, m_nearZ, m_farZ);
		//}
		//else {
		//	my_Mesh1.m_basicVertexConstantBufferData.projection =
		//		XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
		//}
		//my_Mesh1.m_basicVertexConstantBufferData.projection = XMMatrixTranspose(my_Mesh1.m_basicVertexConstantBufferData.projection);

		// Constant를 CPU에서 GPU로 복사
		UpdateBuffer(m_d3dContext, my_Mesh1.m_basicVertexConstantBufferData, my_Mesh1.meshes[0]->vertexConstantBuffer);
		UpdateBuffer(m_d3dContext, my_Mesh2.m_basicVertexConstantBufferData, my_Mesh2.meshes[0]->vertexConstantBuffer);

		// light별 fallofEnd fallofStart값을 변경하지 않기 위해 복사해서 사용
		auto basicPixelData{ my_Mesh1.m_basicPixelConstantBufferData };
		auto basicPixelData2 { my_Mesh2.m_basicPixelConstantBufferData };

		// 여러 개 조명 사용 예시
		for (int i = 0; i < MAX_LIGHTS; i++) {
			// 다른 조명 끄기
			if (i != m_lightType) {
				XMVECTOR strength = XMLoadFloat3(&basicPixelData.lights[i].strength);
				XMStoreFloat3(&basicPixelData.lights[i].strength, XMVectorScale(strength, 0.0f));
			}
			else {
				basicPixelData.lights[i] = m_lightFromGUI;
			}
		}

		UpdateBuffer(m_d3dContext, basicPixelData,
			my_Mesh1.meshes[0]->pixelConstantBuffer);
		UpdateBuffer(m_d3dContext, basicPixelData,
			my_Mesh2.meshes[0]->pixelConstantBuffer);
		//// 노멀 벡터 그리기
		//if (m_drawNormals && m_dirtyFlag) {

		//	UpdateBuffer(m_d3dContext, m_normalVertexConstantBufferData,
		//		m_normalLines->vertexConstantBuffer);

		//	m_dirtyFlag = false;

		//}

		// 큐브매핑을 위한 ConstantBuffers
		my_Mesh1.m_basicVertexConstantBufferData.world = XMMatrixIdentity();
		// Transpose()도 생략 가능
		m_cubeMap.UpdateConstantBuffers(m_d3dDevice, m_d3dContext);

		// Mesh의 MVP를 큐브맵의 vertexConstants로 전달
		UpdateBuffer(m_d3dContext, my_Mesh1.m_basicVertexConstantBufferData,
			m_cubeMap.cubeMesh->vertexConstantBuffer);

		my_Mesh1.m_basicPixelConstantBufferData.material.diffuse =
			CreateXMFLOAT3(m_materialDiffuse);
		my_Mesh1.m_basicPixelConstantBufferData.material.specular =
			CreateXMFLOAT3(m_materialSpecular);
		my_Mesh1.m_basicPixelConstantBufferData.material.fresnelR0=
			m_fresnelR0;

		// post effects
		if (m_dirtyflag) {
			m_postProcesses[1]->m_pixelConstData.threshold = m_threshold;
			m_postProcesses[1]->UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
			m_postProcesses.back()->m_pixelConstData.strength = m_strength;
			m_postProcesses.back()->UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
			m_dirtyflag = 0;
		}
	}

	void D3D11Renderer::Render()
	{
		// IA: Input-Assembler stage
		// VS: Vertex Shader
		// PS: Pixel Shader
		// RS: Rasterizer stage
		// OM: Output-Merger stage

		SetViewport();

		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // 검은색으로 클리어
		m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
		m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* rtvs[] = {
			m_renderTargetView.Get(),
			m_indexRenderTargetView.Get()
		};

		m_d3dContext->OMSetRenderTargets(2, rtvs, m_depthStencilView.Get());
		m_d3dContext->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

		// 어떤 쉐이더를 사용할지 설정
		m_d3dContext->VSSetShader(my_Mesh1.m_colorVertexShader.Get(), 0, 0);

		m_d3dContext->PSSetSamplers(0, 1, my_Mesh1.m_samplerState.GetAddressOf());
		m_d3dContext->PSSetShader(my_Mesh1.m_colorPixelShader.Get(), 0, 0);

		if (m_drawAsWire)
			m_d3dContext->RSSetState(m_wiredRasterizerState.Get());
		else
			m_d3dContext->RSSetState(m_solidRasterizerState.Get());

	

		//if (m_drawNormals) {
		//	m_d3dContext->VSSetShader(m_normalVertexShader.Get(), 0, 0);

		//	m_d3dContext->VSSetConstantBuffers(1, 1, m_normalLines->vertexConstantBuffer.GetAddressOf());

		//	m_d3dContext->PSSetShader(m_normalPixelShader.Get(), 0, 0);
		//	// m_d3dContext->IASetInputLayout(m_basicInputLayout.Get());
		//	m_d3dContext->IASetVertexBuffers(
		//		0, 1, m_normalLines->vertexBuffer.GetAddressOf(), &stride,
		//		&offset);
		//	m_d3dContext->IASetIndexBuffer(m_normalLines->indexBuffer.Get(),
		//		DXGI_FORMAT_R32_UINT, 0);

		//	m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		//	m_d3dContext->DrawIndexed(m_normalLines->m_indexCount, 0, 0);
		//}
		my_Mesh1.Render(m_d3dContext);
		//my_Mesh2.Render(m_d3dContext);

		m_cubeMap.Render(m_d3dContext);

		ComPtr<ID3D11Texture2D> backBuffer;
		m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
		m_d3dContext->ResolveSubresource(m_tempTexture.Get(), 0, backBuffer.Get(), 0,
			DXGI_FORMAT_R8G8B8A8_UNORM);

		for (auto& m_postProcess : m_postProcesses) {
			m_postProcess->Render(m_d3dContext);
		}

		m_d3dContext->ResolveSubresource(m_indexTempTexture.Get(), 0,
			m_indexTexture.Get(), 0,
			DXGI_FORMAT_R8G8B8A8_UNORM);

		//1x1 pixel box
		D3D11_BOX box;
		 

		if (m_cursorX >= m_screenWidth)
		{
			box.left = m_screenWidth - 2;
			box.right = m_screenWidth - 1;
		}
		else
		{
			box.left = m_cursorX;
			box.right = m_cursorX + 1;
		}
		if (m_cursorY >= m_screenHeight)
		{
			box.top = m_cursorY;
			box.bottom = m_cursorY - 1;

		}
		else
		{
			box.top = m_cursorY;
			box.bottom = m_screenHeight + 1;
		}

		// if 3D Texture
		box.front = 0;
		box.back = 1;

		m_d3dContext->CopySubresourceRegion(m_indexStagingTexture.Get(), 0, 0, 0, 0,
			m_indexTempTexture.Get(), 0, &box);

		D3D11_MAPPED_SUBRESOURCE ms;
		m_d3dContext->Map(m_indexStagingTexture.Get(), NULL, D3D11_MAP_READ, NULL,
			&ms); // D3D11_MAP_READ 주의
		memcpy(m_pickColor, ms.pData, sizeof(uint8_t) * 4);
		m_d3dContext->Unmap(m_indexStagingTexture.Get(), NULL);

	}

	void D3D11Renderer::UpdateGUI()
	{
		CreateDockingWindow();

		ImGui::Begin("Scene Control");
		//ImGui::Checkbox("Use Rim", reinterpret_cast<bool*>(&m_basicPixelConstantBufferData.useRim));
		//ImGui::SliderFloat("Rim Strength",
		//	&m_basicPixelConstantBufferData.rimStrength, 0.0f,
		//	10.0f);
		//ImGui::Checkbox("Use Smoothstep",
		//	reinterpret_cast<bool*>(&m_basicPixelConstantBufferData.useSmoothstep));
		//ImGui::SliderFloat3("Rim Color", &m_basicPixelConstantBufferData.rimColor.x,
		//	0.0f, 1.0f);
		//ImGui::SliderFloat("Rim Power", &m_basicPixelConstantBufferData.rimPower,
		//	0.01f, 10.0f);

		// ImGui가 측정해주는 Framerate 출력
		ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::Checkbox("Use Texture", &my_Mesh1.m_basicPixelConstantBufferData.useTexture);
		ImGui::Checkbox("usePerspectiveProjection", &m_usePerspectiveProjection);
		ImGui::Checkbox("drawAsWire", &m_drawAsWire);

		m_dirtyflag +=
			ImGui::SliderFloat("Bloom Threshold", &m_threshold, 0.0f, 1.0f);
		m_dirtyflag +=
			ImGui::SliderFloat("Bloom Strength", &m_strength, 0.0f, 3.0f);

		XMFLOAT3 translation = m_modelTranslation;
		XMFLOAT3 rotation = m_modelRotation;

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Transform")) {
			DrawVector3Control("Translation", m_modelTranslation);
			DrawVector3Control("Rotation", m_modelRotation);
			DrawVector3Control("Scale", m_modelScaling, .5f);

			ImGui::TreePop();
		}

		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		if (ImGui::TreeNode("Lights")) {
			ImGui::SliderFloat("Material Shininess",
				&my_Mesh1.m_basicPixelConstantBufferData.material.shininess, 0.01f,
				20.0f);

			if (ImGui::RadioButton("Directional Light", m_lightType == 0)) {
				m_lightType = 0;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Point Light", m_lightType == 1)) {
				m_lightType = 1;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Spot Light", m_lightType == 2)) {
				m_lightType = 2;
			}

			ImGui::ColorEdit3("Material Fresnel", &m_fresnelR0.x);

			ImGui::SliderFloat("Material Diffuse", &m_materialDiffuse, 0.0f, 1.0f);
			ImGui::SliderFloat("Material Specular", &m_materialSpecular, 0.0f, 1.0f);

			ImGui::SliderFloat3("Light Position", &m_lightFromGUI.position.x, -5.0f,
				5.0f);

			ImGui::SliderFloat("Light fallOffStart", &m_lightFromGUI.fallOffStart, 0.0f,
				5.0f);

			ImGui::SliderFloat("Light fallOffEnd", &m_lightFromGUI.fallOffEnd, 0.0f,
				10.0f);

			ImGui::SliderFloat("Light spotPower", &m_lightFromGUI.spotPower, 1.0f,
				512.0f);
			ImGui::TreePop();
		}

		ImGui::End();
	}
}
