#include "D3D11Renderer.h"

namespace graphics
{
	D3D11Renderer::D3D11Renderer() : D3D11Core(), m_basicVertexConstantBufferData() {}

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

		// Texture sampler �����
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

		//D3D11Core::CreateTexture("D:/sp_env.jpg", temp_tex, temp_srv);

		//meshData = Geometry::SubdivideToSphere(1.f, meshData);
		//MeshData meshData = Geometry::MakeCylinder(2.f, 2.f, 2.f,100, 5);
		//MeshData meshData = Geometry::MakeGrid(2.f, 2.f, 25, 25);
		//MeshData meshData = Geometry::MakePlane();

		//m_mesh = std::make_shared<Mesh>();

		//D3D11Core::CreateVertexBuffer(meshData.vertices, m_mesh->vertexBuffer);
		//// �ε��� ���� �����
		//m_mesh->m_indexCount = UINT(meshData.indices.size());
		//D3D11Core::CreateIndexBuffer(meshData.indices, m_mesh->indexBuffer);

		// ConstantBuffer �����
		m_basicVertexConstantBufferData.world = XMMatrixIdentity();
		m_basicVertexConstantBufferData.view = XMMatrixIdentity();
		m_basicVertexConstantBufferData.projection = XMMatrixIdentity();

		ComPtr<ID3D11Buffer> vertexConstantBuffer;
		ComPtr<ID3D11Buffer> pixelConstantBuffer;

		CreateConstantBuffer(m_d3dDevice, m_basicVertexConstantBufferData,
			vertexConstantBuffer);
		CreateConstantBuffer(m_d3dDevice, m_basicPixelConstantBufferData,
			pixelConstantBuffer);

		for (const auto& meshData : meshes) {
			auto newMesh = std::make_shared<Mesh>();
			CreateVertexBuffer(m_d3dDevice, meshData.vertices, newMesh->vertexBuffer);
			newMesh->m_indexCount = UINT(meshData.indices.size());
			D3D11Utilities::CreateIndexBuffer(m_d3dDevice, meshData.indices, newMesh->indexBuffer);

			if (!meshData.textureFilename.empty()) {

				std::cout << meshData.textureFilename << '\n';
				D3D11Core::CreateTexture(meshData.textureFilename, newMesh->texture,
					newMesh->textureResourceView);
			}

			newMesh->vertexConstantBuffer = vertexConstantBuffer;
			newMesh->pixelConstantBuffer = pixelConstantBuffer;

			this->m_meshes.push_back(newMesh);
		}

		// ���̴� �����

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
			// 4 * 3 �� POSITION = float[3], 4byte*3 ���� �����ͺ��� ����
		};

		D3D11Utilities::CreateVertexShaderAndInputLayout(m_d3dDevice, L"BasicVertexShader.hlsl", inputElements, m_colorVertexShader,
			m_colorInputLayout);

		D3D11Utilities::CreatePixelShader(m_d3dDevice, L"BasicPixelShader.hlsl", m_colorPixelShader);


		// ��� ���� �׸���
		// ������ �ܼ�ȭ�ϱ� ���� InputLayout�� BasicVertexShader�� ���� ����սô�.
		m_normalLines = std::make_shared<Mesh>();
		// Mesh *m_normalLines = new Mesh();

		std::vector<Vertex> normalVertices;
		std::vector<uint32_t> normalIndices;
		size_t offset = 0;
		for (const auto& meshData : meshes) {
			for (size_t i = 0; i < meshData.vertices.size(); i++) {

				auto v = meshData.vertices[i];

				v.texcoord.x = 0.0f; // ������ ǥ��
				normalVertices.push_back(v);

				v.texcoord.x = 1.0f; // ���� ǥ��
				normalVertices.push_back(v);

				normalIndices.push_back(uint32_t(2 * (i + offset)));
				normalIndices.push_back(uint32_t(2 * (i + offset) + 1));
			}
			offset += meshData.vertices.size();
		}

		CreateVertexBuffer(m_d3dDevice, normalVertices, m_normalLines->vertexBuffer);
		m_normalLines->m_indexCount = UINT(normalIndices.size());
		D3D11Utilities::CreateIndexBuffer(m_d3dDevice, normalIndices, m_normalLines->indexBuffer);
		CreateConstantBuffer(m_d3dDevice, m_normalVertexConstantBufferData,
			m_normalLines->vertexConstantBuffer);

		D3D11Utilities::CreateVertexShaderAndInputLayout(m_d3dDevice,
			L"NormalVertexShader.hlsl", inputElements, m_normalVertexShader,
			m_colorInputLayout);
		D3D11Utilities::CreatePixelShader(m_d3dDevice, L"NormalPixelShader.hlsl", m_normalPixelShader);

		//Initialize Success
		return true;
	}


	void D3D11Renderer::Update(float dt)
	{
		using namespace DirectX;

		//auto basicVertexData{ m_basicVertexConstantBufferData };

		// ���� ��ȯ
		m_basicVertexConstantBufferData.world = XMMatrixScaling(m_modelScaling.x, m_modelScaling.y, m_modelScaling.z) *
			XMMatrixRotationY(m_modelRotation.y) *
			XMMatrixRotationX(m_modelRotation.x) *
			XMMatrixRotationZ(m_modelRotation.z) *
			XMMatrixTranslation(m_modelTranslation.x, m_modelTranslation.y, m_modelTranslation.z);
		m_basicVertexConstantBufferData.world = XMMatrixTranspose(m_basicVertexConstantBufferData.world);

		// ������� ��ġ ���
		m_basicVertexConstantBufferData.invTranspose = m_basicVertexConstantBufferData.world;
		m_basicVertexConstantBufferData.invTranspose *= XMMatrixTranslation(m_modelTranslation.x, m_modelTranslation.y, m_modelTranslation.z);
		m_basicVertexConstantBufferData.invTranspose =
			XMMatrixInverse(nullptr, XMMatrixTranspose(m_basicVertexConstantBufferData.invTranspose));


		// ���� ��ȯ
		m_basicVertexConstantBufferData.view =
			XMMatrixRotationX(m_viewRot.x) *
			XMMatrixRotationY(m_viewRot.y) *
			XMMatrixRotationZ(m_viewRot.z) *
			XMMatrixTranslation(0.0f, 0.0f, 2.0f);


		XMStoreFloat3(&m_basicPixelConstantBufferData.eyeWorld, XMVector3Transform(
			XMVectorZero(), XMMatrixInverse(nullptr, m_basicVertexConstantBufferData.view)));

		m_basicVertexConstantBufferData.view =
			XMMatrixTranspose(m_basicVertexConstantBufferData.view);

		// ��������
		m_aspect = D3D11Core::GetAspectRatio();
		if (m_usePerspectiveProjection) {
			m_basicVertexConstantBufferData.projection =
				XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY), m_aspect, m_nearZ, m_farZ);
		}
		else {
			m_basicVertexConstantBufferData.projection =
				XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
		}
		m_basicVertexConstantBufferData.projection = XMMatrixTranspose(m_basicVertexConstantBufferData.projection);

		// Constant�� CPU���� GPU�� ����
		UpdateBuffer(m_d3dContext, m_basicVertexConstantBufferData, m_meshes[0]->vertexConstantBuffer);

		auto basicPixelData{ m_basicPixelConstantBufferData };

		// ���� �� ���� ��� ����
		for (int i = 0; i < MAX_LIGHTS; i++) {
			// �ٸ� ���� ����
			if (i != m_lightType) {
				XMVECTOR strength = XMLoadFloat3(&basicPixelData.lights[i].strength);
				XMStoreFloat3(&basicPixelData.lights[i].strength, XMVectorScale(strength, 0.0f));
			}
			else {
				basicPixelData.lights[i] = m_lightFromGUI;
			}
		}

		UpdateBuffer(m_d3dContext, basicPixelData,
			m_meshes[0]->pixelConstantBuffer);

		// ��� ���� �׸���
		if (m_drawNormals && m_dirtyFlag) {

			UpdateBuffer(m_d3dContext, m_normalVertexConstantBufferData,
				m_normalLines->vertexConstantBuffer);

			m_dirtyFlag = false;

		}

		// ť������� ���� ConstantBuffers
		m_basicVertexConstantBufferData.world = XMMatrixIdentity();
		// Transpose()�� ���� ����
		m_cubeMap.UpdateConstantBuffers(m_d3dDevice, m_d3dContext);

		UpdateBuffer(m_d3dContext, m_basicVertexConstantBufferData,
			m_cubeMap.cubeMesh->vertexConstantBuffer);

		m_basicPixelConstantBufferData.material.diffuse =
			CreateXMFLOAT3(m_materialDiffuse);
		m_basicPixelConstantBufferData.material.specular =
			CreateXMFLOAT3(m_materialSpecular);
		m_basicPixelConstantBufferData.material.fresnelR0=
			m_fresnelR0;
	}

	void D3D11Renderer::Render()
	{
		// IA: Input-Assembler stage
		// VS: Vertex Shader
		// PS: Pixel Shader
		// RS: Rasterizer stage
		// OM: Output-Merger stage

		SetViewport();

		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // ���������� Ŭ����
		m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
		m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
		m_d3dContext->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

		// � ���̴��� ������� ����
		m_d3dContext->VSSetShader(m_colorVertexShader.Get(), 0, 0);

		m_d3dContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
		m_d3dContext->PSSetShader(m_colorPixelShader.Get(), 0, 0);

		if (m_drawAsWire)
			m_d3dContext->RSSetState(m_wiredRasterizerState.Get());
		else
			m_d3dContext->RSSetState(m_solidRasterizerState.Get());

		UINT stride = sizeof(Vertex);
		UINT offset = 0;



		// ���ؽ�/�ε��� ���� ����
		for (const auto& mesh : m_meshes) {
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

		m_cubeMap.Render(m_d3dContext);
	}

	void D3D11Renderer::UpdateGUI()
	{
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
				&m_basicPixelConstantBufferData.material.shininess, 0.01f,
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

			ImGui::SliderFloat3("Material Fresnel", &m_fresnelR0.x, 0.f, 1.f);

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
