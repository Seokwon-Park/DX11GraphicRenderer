#include "D3D11Renderer.h"

namespace graphics
{
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
		blurFilter->SetShaderResources({ m_shaderResourceView });
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

		std::vector<MeshData> meshes = { Geometry::MakeSphere(1.f, 20, 20) };
		meshes[0].textureFilename = "d:/earth.jpg";

		if (!my_Mesh1.Intialize(m_d3dDevice,meshes))
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
			XMMatrixRotationY(m_modelRotation.y) *
			XMMatrixRotationX(m_modelRotation.x) *
			XMMatrixRotationZ(m_modelRotation.z) *
			XMMatrixTranslation(m_modelTranslation.x, m_modelTranslation.y, m_modelTranslation.z);
		my_Mesh1.m_basicVertexConstantBufferData.world = XMMatrixTranspose(my_Mesh1.m_basicVertexConstantBufferData.world);

		m_matrix = my_Mesh1.m_basicVertexConstantBufferData.world;
		// 역행렬의 전치 행렬
		my_Mesh1.m_basicVertexConstantBufferData.invTranspose = my_Mesh1.m_basicVertexConstantBufferData.world;
		my_Mesh1.m_basicVertexConstantBufferData.invTranspose *= XMMatrixTranslation(m_modelTranslation.x, m_modelTranslation.y, m_modelTranslation.z);
		my_Mesh1.m_basicVertexConstantBufferData.invTranspose =
			XMMatrixInverse(nullptr, XMMatrixTranspose(my_Mesh1.m_basicVertexConstantBufferData.invTranspose));

		XMMATRIX viewRow = m_camera.GetViewRow();
		XMMATRIX projRow = m_camera.GetProjRow();
		XMFLOAT3 eyeWorld = m_camera.GetEyePos();

		// 시점 변환

		my_Mesh1.m_basicPixelConstantBufferData.useTexture = true;
		my_Mesh1.m_basicPixelConstantBufferData.eyeWorld = eyeWorld;
		my_Mesh1.m_basicVertexConstantBufferData.view = XMMatrixTranspose(viewRow);
		my_Mesh1.m_basicVertexConstantBufferData.projection =
			XMMatrixTranspose(projRow);

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

		// light별 fallofEnd fallofStart값을 변경하지 않기 위해 복사해서 사용
		auto basicPixelData{ my_Mesh1.m_basicPixelConstantBufferData };

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

		m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
		m_d3dContext->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

		// 어떤 쉐이더를 사용할지 설정
		m_d3dContext->VSSetShader(my_Mesh1.m_colorVertexShader.Get(), 0, 0);

		m_d3dContext->PSSetSamplers(0, 1, my_Mesh1.m_samplerState.GetAddressOf());
		m_d3dContext->PSSetShader(my_Mesh1.m_colorPixelShader.Get(), 0, 0);

		if (m_drawAsWire)
			m_d3dContext->RSSetState(m_wiredRasterizerState.Get());
		else
			m_d3dContext->RSSetState(m_solidRasterizerState.Get());

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		// 버텍스/인덱스 버퍼 설정
		for (const auto& mesh : my_Mesh1.meshes) {
			m_d3dContext->VSSetConstantBuffers(
				0, 1, mesh->vertexConstantBuffer.GetAddressOf());

			ID3D11ShaderResourceView* srv[3] = {
				mesh->textureResourceView.Get(),
				m_cubeMap.diffuseSRV.Get(),
				m_cubeMap.specularSRV.Get()
			};

			m_d3dContext->PSSetShaderResources(
				0, 3, srv);

			m_d3dContext->PSSetConstantBuffers(
				0, 1, mesh->pixelConstantBuffer.GetAddressOf());

			m_d3dContext->IASetInputLayout(my_Mesh1.m_colorInputLayout.Get());
			m_d3dContext->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(),
				&stride, &offset);
			m_d3dContext->IASetIndexBuffer(mesh->indexBuffer.Get(),
				DXGI_FORMAT_R32_UINT, 0);
			m_d3dContext->IASetPrimitiveTopology(
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_d3dContext->DrawIndexed(mesh->m_indexCount, 0, 0);
		}

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

		m_cubeMap.Render(m_d3dContext);

		for (auto& m_postProcess : m_postProcesses) {
			m_postProcess->Render(m_d3dContext);
		}

	}

	void D3D11Renderer::UpdateGUI()
	{
		static bool show_app_fullscreen = false;
		bool* p_open= &show_app_fullscreen;

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
		static ImGuizmo::OPERATION operation(ImGuizmo::TRANSLATE);
		static ImGuizmo::MODE mode(ImGuizmo::LOCAL);

		if (ImGui::IsKeyPressed(ImGuiKey_Z))
			operation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_E))
			operation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
			operation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", operation == ImGuizmo::TRANSLATE))
			operation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", operation == ImGuizmo::ROTATE))
			operation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", operation == ImGuizmo::SCALE))
			operation = ImGuizmo::SCALE;

		// ImGui가 측정해주는 Framerate 출력
		ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


		m_dirtyflag +=
			ImGui::SliderFloat("Bloom Threshold", &m_threshold, 0.0f, 1.0f);
		m_dirtyflag +=
			ImGui::SliderFloat("Bloom Strength", &m_strength, 0.0f, 3.0f);


		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		if (ImGui::TreeNode("General")) {
			ImGui::Checkbox("Use Texture", &my_Mesh1.m_basicPixelConstantBufferData.useTexture);
			ImGui::Checkbox("usePerspectiveProjection", &m_usePerspectiveProjection);
			ImGui::Checkbox("drawAsWire", &m_drawAsWire);
			//ImGui::Checkbox("drawNormals", &m_drawNormals);
			//if (ImGui::SliderFloat("Normal scale",
			//	&m_normalVertexConstantBufferData.scale, 0.0f,
			//	1.0f)) {
			//	m_dirtyFlag = true;
			//}
			
			XMMATRIX transform = XMMatrixIdentity(); // 초기화된 단위 행렬

			// 이동, 회전 및 스케일을 변환 행렬에 적용합니다.
			transform *= XMMatrixTranslationFromVector(XMLoadFloat3(&m_modelTranslation));
			transform *= XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_modelRotation));
			transform *= XMMatrixScalingFromVector(XMLoadFloat3(&m_modelScaling));
			
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

			float matrix[16];
			XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(matrix), transform);

			float proj[16];
			XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(proj), m_camera.GetProjRow());

			float view_mat[16];
			XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(view_mat), m_camera.GetViewRow());
			//// ImGuizmo를 사용하여 행렬 편집 위젯을 그립니다.
			ImGuizmo::Manipulate(
				/* view matrix */ view_mat,
				/* projection matrix */ proj,
				operation,
				mode,
				matrix
			);

				transform = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(matrix));
			//// 변경된 행렬을 다시 XMMATRIX로 변환합니다.
			if (ImGuizmo::IsUsing())
			{

				XMVECTOR trans;
				XMVECTOR rot;
				XMVECTOR scale; // 스케일을 저장할 XMVECTOR

				XMMatrixDecompose(&scale, &rot, &trans, transform);

				XMStoreFloat3(&m_modelTranslation, trans);
				XMStoreFloat3(&m_modelRotation, rot);
				XMStoreFloat3(&m_modelScaling, scale);
			}

			ImGui::SliderFloat3("Translation", &m_modelTranslation.x, -2.0f, 2.0f);
			ImGui::SliderFloat3("Rotation", &m_modelRotation.x, -3.14f, 3.14f);
			ImGui::SliderFloat3("Scaling", &m_modelScaling.x, 0.1f, 2.0f);

			ImGui::SliderFloat3("m_viewRot", &m_viewRot.x, -3.14f, 3.14f);

			//ImGui::SliderFloat3("m_viewEyePos", &m_viewEyePos.x, -4.0f, 4.0f);
			//ImGui::SliderFloat3("m_viewEyeDir", &m_viewEyeDir.x, -4.0f, 4.0f);
			//ImGui::SliderFloat3("m_viewUp", &m_viewUp.x, -2.0f, 2.0f);

			//ImGui::SliderFloat("m_projFovAngleY(Deg)", &m_projFovAngleY, 10.0f, 180.0f);
			//ImGui::SliderFloat("m_nearZ", &m_nearZ, 0.01f, m_farZ - 0.001f);
			//ImGui::SliderFloat("m_farZ", &m_farZ, m_nearZ + 0.01f, 10.0f);
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
