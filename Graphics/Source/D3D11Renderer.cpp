#include "D3D11Renderer.h"

#include <directxtk/DDSTextureLoader.h>

namespace graphics
{
	D3D11Renderer::D3D11Renderer() : D3D11EngineBase(), m_basicVertexConstantBufferData() {}

	void D3D11Renderer::InitializeCubeMapping() {

		// texassemble.exe cube -w 2048 -h 2048 -o saintpeters.dds posx.jpg negx.jpg
		// posy.jpg negy.jpg posz.jpg negz.jpg texassemble.exe cube -w 2048 -h 2048
		// -o skybox.dds right.jpg left.jpg top.jpg bottom.jpg front.jpg back.jpg -y
		// https://github.com/Microsoft/DirectXTex/wiki/Texassemble

		// .dds 파일 읽어들여서 초기화
		ComPtr<ID3D11Texture2D> texture;
		auto hr = CreateDDSTextureFromFileEx(
			// this->m_device.Get(), L"./SaintPetersBasilica/saintpeters.dds", 0,
			this->m_d3dDevice.Get(), L"d:/CloudCommon_diffuseIBL.dds", 0, D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE, 0,
			D3D11_RESOURCE_MISC_TEXTURECUBE, // 큐브맵용 텍스춰
			DDS_LOADER_FLAGS(false), (ID3D11Resource**)texture.GetAddressOf(),
			this->m_cubeMapping.diffuseSRV.GetAddressOf(), nullptr);

		hr = CreateDDSTextureFromFileEx(
			// this->m_device.Get(), L"./SaintPetersBasilica/saintpeters.dds", 0,
			this->m_d3dDevice.Get(), L"d:/CloudCommons_specularIBL.dds", 0, D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE, 0,
			D3D11_RESOURCE_MISC_TEXTURECUBE, // 큐브맵용 텍스춰
			DDS_LOADER_FLAGS(false), (ID3D11Resource**)texture.GetAddressOf(),
			this->m_cubeMapping.specularSRV.GetAddressOf(), nullptr);

		if (FAILED(hr)) {
			std::cout << "CreateDDSTextureFromFileEx() failed" << std::endl;
		}

		m_cubeMapping.cubeMesh = std::make_shared<Mesh>();

		m_basicVertexConstantBufferData.world = XMMatrixIdentity();
		m_basicVertexConstantBufferData.view = XMMatrixIdentity();
		m_basicVertexConstantBufferData.projection = XMMatrixIdentity();
		ComPtr<ID3D11Buffer> vertexConstantBuffer;
		ComPtr<ID3D11Buffer> pixelConstantBuffer;
		util::CreateConstantBuffer(m_d3dDevice, m_basicVertexConstantBufferData,
			m_cubeMapping.cubeMesh->vertexConstantBuffer);
		util::CreateConstantBuffer(m_d3dDevice, m_basicPixelConstantBufferData,
			m_cubeMapping.cubeMesh->pixelConstantBuffer);

		// 커다란 박스 초기화
		// - 세상이 커다란 박스 안에 갇혀 있는 구조입니다.
		// - D3D11_CULL_MODE::D3D11_CULL_NONE 또는 삼각형 뒤집기
		// - 예시) std::reverse(myvector.begin(),myvector.end());
		//MeshData cubeMeshData = Geometry::MakeIcosahedron();
		//MeshData cubeMeshData = Geometry::MakeTetrahedron();
		MeshData cubeMeshData = Geometry::MakeSphere(20.f, 10, 10);
		//MeshData cubeMeshData = Geometry::MakeSphere(1.f, 10, 10);
		//cubeMeshData = Geometry::SubdivideToSphere(15.f, cubeMeshData);
		//cubeMeshData = Geometry::SubdivideToSphere(15.f, cubeMeshData);
		//cubeMeshData = Geometry::SubdivideToSphere(15.f, cubeMeshData);
		//cubeMeshData = Geometry::SubdivideToSphere(15.f, cubeMeshData);

		std::reverse(cubeMeshData.indices.begin(), cubeMeshData.indices.end());

		util::CreateVertexBuffer(m_d3dDevice, cubeMeshData.vertices,
			m_cubeMapping.cubeMesh->vertexBuffer);
		m_cubeMapping.cubeMesh->m_indexCount = UINT(cubeMeshData.indices.size());
		CreateIndexBuffer(cubeMeshData.indices,
			m_cubeMapping.cubeMesh->indexBuffer);

		// 쉐이더 초기화

		// 다른 쉐이더와 동일한 InputLayout 입니다.
		// 실제로는 "POSITION"만 사용합니다.
		std::vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
			 D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
			 D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3,
			 D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		D3D11EngineBase::CreateVertexShaderAndInputLayout(
			L"CubeVS.hlsl", basicInputElements,
			m_cubeMapping.vertexShader, m_cubeMapping.inputLayout);

		D3D11EngineBase::CreatePixelShader(L"CubePS.hlsl",
			m_cubeMapping.pixelShader);

		// 기타
		// - 텍스춰 샘플러도 다른 텍스춰와 같이 사용
	}

	bool D3D11Renderer::Initialize()
	{
		// Initialize Essentials
		if (!D3D11EngineBase::Initialize())
			return false;

		InitializeCubeMapping();

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
		//std::vector<MeshData> meshes = { Geometry::MakeIcosahedron() };
		//std::vector<MeshData> meshes = { Geometry::MakeCube(1,1,1) };
		//std::vector<MeshData> meshes = { Geometry::MakeCylinder(.5f,.5f,3, 10,1) };
		std::vector<MeshData> meshes = { Geometry::MakeSphere(1.f, 10, 10) };
		for (auto& meshData : meshes) {
			meshData = Geometry::SubdivideToSphere(1.5f, meshData);
			meshData = Geometry::SubdivideToSphere(1.5f, meshData);
			meshData = Geometry::SubdivideToSphere(1.5f, meshData);
			meshData = Geometry::SubdivideToSphere(1.5f, meshData);
		}

		meshes[0].textureFilename = "D:/earth.jpg";

		//D3D11EngineBase::CreateTexture("D:/sp_env.jpg", temp_tex, temp_srv);

		//meshData = Geometry::SubdivideToSphere(1.f, meshData);
		//MeshData meshData = Geometry::MakeCylinder(2.f, 2.f, 2.f,100, 5);
		//MeshData meshData = Geometry::MakeGrid(2.f, 2.f, 25, 25);
		//MeshData meshData = Geometry::MakePlane();

		//m_mesh = std::make_shared<Mesh>();

		//D3D11EngineBase::CreateVertexBuffer(meshData.vertices, m_mesh->vertexBuffer);
		//// 인덱스 버퍼 만들기
		//m_mesh->m_indexCount = UINT(meshData.indices.size());
		//D3D11EngineBase::CreateIndexBuffer(meshData.indices, m_mesh->indexBuffer);

		// ConstantBuffer 만들기
		m_basicVertexConstantBufferData.world = XMMatrixIdentity();
		m_basicVertexConstantBufferData.view = XMMatrixIdentity();
		m_basicVertexConstantBufferData.projection = XMMatrixIdentity();

		ComPtr<ID3D11Buffer> vertexConstantBuffer;
		ComPtr<ID3D11Buffer> pixelConstantBuffer;

		util::CreateConstantBuffer(m_d3dDevice, m_basicVertexConstantBufferData,
			vertexConstantBuffer);
		util::CreateConstantBuffer(m_d3dDevice, m_basicPixelConstantBufferData,
			pixelConstantBuffer);

		for (const auto& meshData : meshes) {
			auto newMesh = std::make_shared<Mesh>();
			util::CreateVertexBuffer(m_d3dDevice, meshData.vertices, newMesh->vertexBuffer);
			newMesh->m_indexCount = UINT(meshData.indices.size());
			CreateIndexBuffer(meshData.indices, newMesh->indexBuffer);

			if (!meshData.textureFilename.empty()) {

				std::cout << meshData.textureFilename << '\n';
				D3D11EngineBase::CreateTexture(meshData.textureFilename, newMesh->texture,
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

		D3D11EngineBase::CreateVertexShaderAndInputLayout(L"BasicVertexShader.hlsl", inputElements, m_colorVertexShader,
			m_colorInputLayout);

		D3D11EngineBase::CreatePixelShader(L"BasicPixelShader.hlsl", m_colorPixelShader);


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

		util::CreateVertexBuffer(m_d3dDevice, normalVertices, m_normalLines->vertexBuffer);
		m_normalLines->m_indexCount = UINT(normalIndices.size());
		CreateIndexBuffer(normalIndices, m_normalLines->indexBuffer);
		util::CreateConstantBuffer(m_d3dDevice, m_normalVertexConstantBufferData,
			m_normalLines->vertexConstantBuffer);

		D3D11EngineBase::CreateVertexShaderAndInputLayout(
			L"NormalVertexShader.hlsl", inputElements, m_normalVertexShader,
			m_colorInputLayout);
		D3D11EngineBase::CreatePixelShader(L"NormalPixelShader.hlsl", m_normalPixelShader);

		return true;
	}


	void D3D11Renderer::Update(float dt)
	{
		using namespace DirectX;

		//auto basicVertexData{ m_basicVertexConstantBufferData };

		// 모델의 변환
		m_basicVertexConstantBufferData.world = XMMatrixScaling(m_modelScaling.x, m_modelScaling.y, m_modelScaling.z) *
			XMMatrixRotationY(m_modelRotation.y) *
			XMMatrixRotationX(m_modelRotation.x) *
			XMMatrixRotationZ(m_modelRotation.z) *
			XMMatrixTranslation(m_modelTranslation.x, m_modelTranslation.y, m_modelTranslation.z);
		m_basicVertexConstantBufferData.world = XMMatrixTranspose(m_basicVertexConstantBufferData.world);

		// 역행렬의 전치 행렬
		m_basicVertexConstantBufferData.invTranspose = m_basicVertexConstantBufferData.world;
		m_basicVertexConstantBufferData.invTranspose *= XMMatrixTranslation(m_modelTranslation.x, m_modelTranslation.y, m_modelTranslation.z);
		m_basicVertexConstantBufferData.invTranspose =
			XMMatrixInverse(nullptr, XMMatrixTranspose(m_basicVertexConstantBufferData.invTranspose));


		// 시점 변환
		m_basicVertexConstantBufferData.view =
			XMMatrixRotationX(m_viewRot.x) *
			XMMatrixRotationY(m_viewRot.y) *
			XMMatrixRotationZ(m_viewRot.z) *
			XMMatrixTranslation(0.0f, 0.0f, 2.0f);


		XMStoreFloat3(&m_basicPixelConstantBufferData.eyeWorld, XMVector3Transform(
			XMVectorZero(), XMMatrixInverse(nullptr, m_basicVertexConstantBufferData.view)));

		m_basicVertexConstantBufferData.view =
			XMMatrixTranspose(m_basicVertexConstantBufferData.view);

		// 프로젝션
		m_aspect = D3D11EngineBase::GetAspectRatio();
		if (m_usePerspectiveProjection) {
			m_basicVertexConstantBufferData.projection =
				XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY), m_aspect, m_nearZ, m_farZ);
		}
		else {
			m_basicVertexConstantBufferData.projection =
				XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
		}
		m_basicVertexConstantBufferData.projection = XMMatrixTranspose(m_basicVertexConstantBufferData.projection);

		// Constant를 CPU에서 GPU로 복사
		util::UpdateBuffer(m_d3dContext, m_basicVertexConstantBufferData, m_meshes[0]->vertexConstantBuffer);

		auto basicPixelData{ m_basicPixelConstantBufferData };

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

		util::UpdateBuffer(m_d3dContext, basicPixelData,
			m_meshes[0]->pixelConstantBuffer);

		// 노멀 벡터 그리기
		if (m_drawNormals && m_dirtyFlag) {

			util::UpdateBuffer(m_d3dContext, m_normalVertexConstantBufferData,
				m_normalLines->vertexConstantBuffer);

			m_dirtyFlag = false;

		}

		// 큐브매핑을 위한 ConstantBuffers
		m_basicVertexConstantBufferData.world = XMMatrixIdentity();
		// Transpose()도 생략 가능

		util::UpdateBuffer(m_d3dContext, m_basicVertexConstantBufferData,
			m_cubeMapping.cubeMesh->vertexConstantBuffer);

		m_basicPixelConstantBufferData.material.diffuse =
			CreateXMFLOAT3(m_materialDiffuse);
		m_basicPixelConstantBufferData.material.specular =
			CreateXMFLOAT3(m_materialSpecular);
	}

	void D3D11Renderer::Render()
	{
		// IA: Input-Assembler stage
		// VS: Vertex Shader
		// PS: Pixel Shader
		// RS: Rasterizer stage
		// OM: Output-Merger stage

		SetViewport();

		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
		m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
		m_d3dContext->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

		// 어떤 쉐이더를 사용할지 설정
		m_d3dContext->VSSetShader(m_colorVertexShader.Get(), 0, 0);

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

			ID3D11ShaderResourceView* srv[3] = {
				mesh->textureResourceView.Get(),
				m_cubeMapping.diffuseSRV.Get(),
				m_cubeMapping.specularSRV.Get()
			};

			m_d3dContext->PSSetShaderResources(
				0, 3, srv);

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

		// 큐브매핑
		// 맨 끝에서 렌더링을 진행해도 Depth Buffer가 처리해준다.
		m_d3dContext->IASetInputLayout(m_cubeMapping.inputLayout.Get());
		m_d3dContext->IASetVertexBuffers(
			0, 1, m_cubeMapping.cubeMesh->vertexBuffer.GetAddressOf(), &stride,
			&offset);
		m_d3dContext->IASetIndexBuffer(m_cubeMapping.cubeMesh->indexBuffer.Get(),
			DXGI_FORMAT_R32_UINT, 0);
		m_d3dContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_d3dContext->VSSetShader(m_cubeMapping.vertexShader.Get(), 0, 0);
		m_d3dContext->VSSetConstantBuffers(
			0, 1, m_cubeMapping.cubeMesh->vertexConstantBuffer.GetAddressOf());

		//ID3D11ShaderResourceView* views[1] = {
		//	m_cubeMapping.specularSRV.Get() };
		ID3D11ShaderResourceView* views[2] = {
			m_cubeMapping.diffuseSRV.Get(),
			m_cubeMapping.specularSRV.Get()
		};
		m_d3dContext->PSSetShaderResources(0, 2, views);

		m_d3dContext->PSSetShader(m_cubeMapping.pixelShader.Get(), 0, 0);
		m_d3dContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
		m_d3dContext->PSSetConstantBuffers(
			0, 1, m_cubeMapping.cubeMesh->pixelConstantBuffer.GetAddressOf());

		m_d3dContext->DrawIndexed(m_cubeMapping.cubeMesh->m_indexCount, 0, 0);
	}

	void D3D11Renderer::UpdateGUI()
	{
		ImGui::Checkbox("Use Rim", reinterpret_cast<bool*>(&m_basicPixelConstantBufferData.useRim));
		ImGui::SliderFloat("Rim Strength",
			&m_basicPixelConstantBufferData.rimStrength, 0.0f,
			10.0f);
		ImGui::Checkbox("Use Smoothstep",
			reinterpret_cast<bool*>(&m_basicPixelConstantBufferData.useSmoothstep));
		ImGui::SliderFloat3("Rim Color", &m_basicPixelConstantBufferData.rimColor.x,
			0.0f, 1.0f);
		ImGui::SliderFloat("Rim Power", &m_basicPixelConstantBufferData.rimPower,
			0.01f, 10.0f);

		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
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
