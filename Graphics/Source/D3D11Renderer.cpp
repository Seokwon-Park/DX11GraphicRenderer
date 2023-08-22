#include "D3D11Renderer.h"

namespace graphics
{
	D3D11Renderer::D3D11Renderer() : D3D11AppBase(), m_basicVertexConstantBufferData() {}

	bool D3D11Renderer::Initialize()
	{
		// Initialize Essentials
		if (!D3D11AppBase::Initialize())
			return false;

		//Fbx SDK -> 보류

		// Texture sampler 만들기
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		// Create the Sample State
		m_d3dDevice->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf());

		//std::vector<MeshData> meshes = { Geometry::MakeSphere(1.f, 4, 4) };
		//for (auto& meshData : meshes) {
		//	meshData = Geometry::SubdivideToSphere(1.f, meshData);
		//	meshData = Geometry::SubdivideToSphere(1.f, meshData);
		//	meshData = Geometry::SubdivideToSphere(1.f, meshData);
		//	meshData = Geometry::SubdivideToSphere(1.f, meshData);
		//}


		//std::vector<MeshData> meshes = { Geometry::MakePlane(2.f, Axis::x, Axis::y) };
		//std::vector<MeshData> meshes = Geometry::ReadModelFromFile("c:/zelda/zeldaPosed001.fbx");
		//std::vector<MeshData> meshes = { Geometry::MakeTetrahedron(1.f) };
		//std::vector<MeshData> meshes = { Geometry::MakeIcosahedron(1.f) };
		std::vector<MeshData> meshes = { Geometry::MakeCube(1,1,1,1) };
		//for (auto& meshData : meshes) {
		//	meshData = Geometry::SubdivideToSphere(1.5f, meshData);
		//	meshData = Geometry::SubdivideToSphere(1.5f, meshData);
		//	meshData = Geometry::SubdivideToSphere(1.5f, meshData);
		//	meshData = Geometry::SubdivideToSphere(1.5f, meshData);
		//}

		meshes[0].textureFilename = "D:/earth.jpg";

		//meshData = Geometry::SubdivideToSphere(1.f, meshData);
		//MeshData meshData = Geometry::MakeCylinder(2.f, 2.f, 2.f,100, 5);
		//MeshData meshData = Geometry::MakeGrid(2.f, 2.f, 25, 25);
		//MeshData meshData = Geometry::MakePlane();

		//m_mesh = std::make_shared<Mesh>();

		//D3D11AppBase::CreateVertexBuffer(meshData.vertices, m_mesh->vertexBuffer);
		//// 인덱스 버퍼 만들기
		//m_mesh->m_indexCount = UINT(meshData.indices.size());
		//D3D11AppBase::CreateIndexBuffer(meshData.indices, m_mesh->indexBuffer);

		// ConstantBuffer 만들기
		m_basicVertexConstantBufferData.world = XMMATRIX();
		m_basicVertexConstantBufferData.view = XMMATRIX();
		m_basicVertexConstantBufferData.projection = XMMATRIX();

		ComPtr<ID3D11Buffer> vertexConstantBuffer;
		ComPtr<ID3D11Buffer> pixelConstantBuffer;

		D3D11AppBase::CreateConstantBuffer(m_basicVertexConstantBufferData,
			vertexConstantBuffer);
		D3D11AppBase::CreateConstantBuffer(m_basicPixelConstantBufferData,
			pixelConstantBuffer);

		for (const auto& meshData : meshes) {
			auto newMesh = std::make_shared<Mesh>();
			D3D11AppBase::CreateVertexBuffer(meshData.vertices, newMesh->vertexBuffer);
			newMesh->m_indexCount = UINT(meshData.indices.size());
			D3D11AppBase::CreateIndexBuffer(meshData.indices, newMesh->indexBuffer);

			if (!meshData.textureFilename.empty()) {

				std::cout << meshData.textureFilename << '\n';
				D3D11AppBase::CreateTexture(meshData.textureFilename, newMesh->texture,
					newMesh->textureResourceView);
			}

			newMesh->vertexConstantBuffer = vertexConstantBuffer;
			newMesh->pixelConstantBuffer = pixelConstantBuffer;

			this->m_meshes.push_back(newMesh);
		}

		// 쉐이더 만들기

		// Input-layout objects describe how vertex buffer data is streamed into the
		// IA(Input-Assembler) pipeline stage.
		// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetinputlayout

		// Input-Assembler Stage
		// The purpose of the input-assembler stage is to read primitive data
		// (points, lines and/or triangles) from user-filled buffers and assemble
		// the data into primitives that will be used by the other pipeline stages.
		// https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-input-assembler-stage

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
			// 4 * 3 은 POSITION = float[3], 4byte*3 다음 데이터부터 시작
		};

		D3D11AppBase::CreateVertexShaderAndInputLayout(L"BasicVertexShader.hlsl", inputElements, m_colorVertexShader,
			m_colorInputLayout);

		D3D11AppBase::CreatePixelShader(L"BasicPixelShader.hlsl", m_colorPixelShader);


		// 노멀 벡터 그리기
		// 문제를 단순화하기 위해 InputLayout은 BasicVertexShader와 같이 사용합시다.
		m_normalLines = std::make_shared<Mesh>();
		// Mesh *m_normalLines = new Mesh();

		std::vector<Vertex> normalVertices;
		std::vector<uint32_t> normalIndices;
		size_t offset = 0;
		for (const auto& meshData : meshes) {
			for (size_t i = 0; i < meshData.vertices.size(); i++) {

				auto v = meshData.vertices[i];

				v.texcoord.x = 0.0f; // 시작점 표시
				normalVertices.push_back(v);

				v.texcoord.x = 1.0f; // 끝점 표시
				normalVertices.push_back(v);

				normalIndices.push_back(uint32_t(2 * (i + offset)));
				normalIndices.push_back(uint32_t(2 * (i + offset) + 1));
			}
			offset += meshData.vertices.size();
		}

		// TODO: 여기에 필요한 내용들 작성
		D3D11AppBase::CreateVertexBuffer(normalVertices, m_normalLines->vertexBuffer);
		m_normalLines->m_indexCount = UINT(normalIndices.size());
		D3D11AppBase::CreateIndexBuffer(normalIndices, m_normalLines->indexBuffer);
		D3D11AppBase::CreateConstantBuffer(m_normalVertexConstantBufferData,
			m_normalLines->vertexConstantBuffer);

		D3D11AppBase::CreateVertexShaderAndInputLayout(
			L"NormalVertexShader.hlsl", inputElements, m_normalVertexShader,
			m_colorInputLayout);
		D3D11AppBase::CreatePixelShader(L"NormalPixelShader.hlsl", m_normalPixelShader);

		return true;
	}


	void D3D11Renderer::Update(float dt)
	{
		using namespace DirectX;

		auto basicVertexData{ m_basicVertexConstantBufferData };
		auto basicPixelData{ m_basicPixelConstantBufferData };

		// 모델의 변환
		basicVertexData.world = XMMatrixScaling(m_modelScaling.x, m_modelScaling.y, m_modelScaling.z) *
			XMMatrixRotationY(m_modelRotation.y) *
			XMMatrixRotationX(m_modelRotation.x) *
			XMMatrixRotationZ(m_modelRotation.z) *
			XMMatrixTranslation(m_modelTranslation.x, m_modelTranslation.y, m_modelTranslation.z);
		basicVertexData.world = XMMatrixTranspose(basicVertexData.world);

		// 역행렬의 전치 행렬
		basicVertexData.invTranspose = basicVertexData.world;
		basicVertexData.invTranspose *= XMMatrixTranslation(m_modelTranslation.x, m_modelTranslation.y, m_modelTranslation.z);
		basicVertexData.invTranspose =
			XMMatrixInverse(nullptr, XMMatrixTranspose(basicVertexData.invTranspose));


		// 시점 변환
		basicVertexData.view =
			XMMatrixRotationY(m_viewRot) *
			XMMatrixTranslation(0.0f, 0.0f, 2.0f);


		XMStoreFloat3(&m_basicPixelConstantBufferData.eyeWorld, XMVector3Transform(
			XMVectorZero(), XMMatrixInverse(nullptr, basicVertexData.view)));

		basicVertexData.view =
			XMMatrixTranspose(basicVertexData.view);

		// 프로젝션
		m_aspect = D3D11AppBase::GetAspectRatio();
		if (m_usePerspectiveProjection) {
			basicVertexData.projection =
				XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY), m_aspect, m_nearZ, m_farZ);
		}
		else {
			basicVertexData.projection =
				XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
		}
		basicVertexData.projection = XMMatrixTranspose(basicVertexData.projection);

		// Constant를 CPU에서 GPU로 복사
		D3D11AppBase::UpdateBuffer(basicVertexData, m_meshes[0]->vertexConstantBuffer);

		basicPixelData.material.diffuse = XMFLOAT3(m_materialDiffuse, m_materialDiffuse, m_materialDiffuse);
		basicPixelData.material.specular = XMFLOAT3(m_materialSpecular, m_materialSpecular, m_materialSpecular);

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

		D3D11AppBase::UpdateBuffer(basicPixelData,
			m_meshes[0]->pixelConstantBuffer);

		// 노멀 벡터 그리기
		if (m_drawNormals && m_dirtyFlag) {

			D3D11AppBase::UpdateBuffer(m_normalVertexConstantBufferData,
				m_normalLines->vertexConstantBuffer);

			m_dirtyFlag = false;

		}
	}

	void D3D11Renderer::Render()
	{
		// IA: Input-Assembler stage
		// VS: Vertex Shader
		// PS: Pixel Shader
		// RS: Rasterizer stage
		// OM: Output-Merger stage

		//m_d3dContext->RSSetViewports(1, &m_screenViewport);
		SetViewport();

		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
		m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// 비교: Depth Buffer를 사용하지 않는 경우
		// m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);
		m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

		m_d3dContext->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

		// 어떤 쉐이더를 사용할지 설정
		m_d3dContext->VSSetShader(m_colorVertexShader.Get(), 0, 0);

		/* 경우에 따라서는 포인터의 배열을 넣어줄 수도 있습니다.
		ID3D11Buffer *pptr[1] = {
			m_constantBuffer.Get(),
		};
		m_d3dContext->VSSetConstantBuffers(0, 1, pptr); */

		m_d3dContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

		m_d3dContext->PSSetShader(m_colorPixelShader.Get(), 0, 0);

		if (m_drawAsWire)
			m_d3dContext->RSSetState(m_wiredRasterizerState.Get());
		else
			m_d3dContext->RSSetState(m_solidRasterizerState.Get());

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		// 버텍스/인덱스 버퍼 설정
		for (const auto& mesh : m_meshes) {
			m_d3dContext->VSSetConstantBuffers(
				0, 1, mesh->vertexConstantBuffer.GetAddressOf());

			m_d3dContext->PSSetShaderResources(
				0, 1, mesh->textureResourceView.GetAddressOf());

			m_d3dContext->PSSetConstantBuffers(
				0, 1, mesh->pixelConstantBuffer.GetAddressOf());

			m_d3dContext->IASetInputLayout(m_colorInputLayout.Get());
			m_d3dContext->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(),
				&stride, &offset);
			m_d3dContext->IASetIndexBuffer(mesh->indexBuffer.Get(),
				DXGI_FORMAT_R32_UINT, 0);
			m_d3dContext->IASetPrimitiveTopology(
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_d3dContext->DrawIndexed(mesh->m_indexCount, 0, 0);
		}

		if (m_drawNormals) {
			m_d3dContext->VSSetShader(m_normalVertexShader.Get(), 0, 0);

			m_d3dContext->VSSetConstantBuffers(1, 1, m_normalLines->vertexConstantBuffer.GetAddressOf());

			m_d3dContext->PSSetShader(m_normalPixelShader.Get(), 0, 0);
			// m_d3dContext->IASetInputLayout(m_basicInputLayout.Get());
			m_d3dContext->IASetVertexBuffers(
				0, 1, m_normalLines->vertexBuffer.GetAddressOf(), &stride,
				&offset);
			m_d3dContext->IASetIndexBuffer(m_normalLines->indexBuffer.Get(),
				DXGI_FORMAT_R32_UINT, 0);

			m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			m_d3dContext->DrawIndexed(m_normalLines->m_indexCount, 0, 0);
		}
	}

	void D3D11Renderer::UpdateGUI()
	{
		ImGui::SliderFloat("Rim Strength",
			&m_basicPixelConstantBufferData.rimStrength, 0.0f,
			10.0f);
		ImGui::Checkbox("Use Smoothstep",
			&m_basicPixelConstantBufferData.useSmoothstep);
		ImGui::SliderFloat3("Rim Color", &m_basicPixelConstantBufferData.rimColor.x,
			0.0f, 1.0f);
		ImGui::SliderFloat("Rim Power", &m_basicPixelConstantBufferData.rimPower,
			0.01f, 10.0f);

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("General")) {
			ImGui::Checkbox("Use Texture", &m_basicPixelConstantBufferData.useTexture);
			ImGui::Checkbox("usePerspectiveProjection", &m_usePerspectiveProjection);
			ImGui::Checkbox("drawAsWire", &m_drawAsWire);
			ImGui::Checkbox("drawNormals", &m_drawNormals);
			if (ImGui::SliderFloat("Normal scale",
				&m_normalVertexConstantBufferData.scale, 0.0f,
				1.0f)) {
				m_dirtyFlag = true;
			}

			ImGui::SliderFloat3("m_modelTranslation", &m_modelTranslation.x, -2.0f, 2.0f);
			ImGui::SliderFloat3("m_modelRotation(Rad)", &m_modelRotation.x, -3.14f, 3.14f);
			ImGui::SliderFloat3("m_modelScaling", &m_modelScaling.x, 0.1f, 2.0f);

			ImGui::SliderFloat("m_viewRot", &m_viewRot, -3.14f, 3.14f);

			//ImGui::SliderFloat3("m_viewEyePos", &m_viewEyePos.x, -4.0f, 4.0f);
			//ImGui::SliderFloat3("m_viewEyeDir", &m_viewEyeDir.x, -4.0f, 4.0f);
			//ImGui::SliderFloat3("m_viewUp", &m_viewUp.x, -2.0f, 2.0f);

			//ImGui::SliderFloat("m_projFovAngleY(Deg)", &m_projFovAngleY, 10.0f, 180.0f);
			//ImGui::SliderFloat("m_nearZ", &m_nearZ, 0.01f, m_farZ - 0.001f);
			//ImGui::SliderFloat("m_farZ", &m_farZ, m_nearZ + 0.01f, 10.0f);
			ImGui::TreePop();
		}

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Lights")) {
			ImGui::SliderFloat("Material Shininess",
				&m_basicPixelConstantBufferData.material.shininess, 1.0f,
				256.0f);

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
	}
}
