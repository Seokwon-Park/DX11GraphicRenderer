#include "D3D11Renderer.h"

namespace graphics
{
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

		if (!meshes.Intialize(m_d3dDevice, "c:/zelda/zeldaPosed001.fbx"))
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

		//Initialize Success
		return true;
	}


	void D3D11Renderer::Update(float dt)
	{
		using namespace DirectX;

		//auto basicVertexData{ m_basicVertexConstantBufferData };

		// 모델의 변환
		meshes.m_basicVertexConstantBufferData.world = XMMatrixScaling(m_modelScaling.x, m_modelScaling.y, m_modelScaling.z) *
			XMMatrixRotationY(m_modelRotation.y) *
			XMMatrixRotationX(m_modelRotation.x) *
			XMMatrixRotationZ(m_modelRotation.z) *
			XMMatrixTranslation(m_modelTranslation.x, m_modelTranslation.y, m_modelTranslation.z);
		meshes.m_basicVertexConstantBufferData.world = XMMatrixTranspose(meshes.m_basicVertexConstantBufferData.world);

		// 역행렬의 전치 행렬
		meshes.m_basicVertexConstantBufferData.invTranspose = meshes.m_basicVertexConstantBufferData.world;
		meshes.m_basicVertexConstantBufferData.invTranspose *= XMMatrixTranslation(m_modelTranslation.x, m_modelTranslation.y, m_modelTranslation.z);
		meshes.m_basicVertexConstantBufferData.invTranspose =
			XMMatrixInverse(nullptr, XMMatrixTranspose(meshes.m_basicVertexConstantBufferData.invTranspose));


		// 시점 변환
		meshes.m_basicVertexConstantBufferData.view =
			XMMatrixRotationX(m_viewRot.x) *
			XMMatrixRotationY(m_viewRot.y) *
			XMMatrixRotationZ(m_viewRot.z) *
			XMMatrixTranslation(0.0f, 0.0f, 2.0f);


		XMStoreFloat3(&meshes.m_basicPixelConstantBufferData.eyeWorld, XMVector3Transform(
			XMVectorZero(), XMMatrixInverse(nullptr, meshes.m_basicVertexConstantBufferData.view)));

		meshes.m_basicVertexConstantBufferData.view =
			XMMatrixTranspose(meshes.m_basicVertexConstantBufferData.view);

		// 프로젝션
		m_aspect = D3D11Core::GetAspectRatio();
		if (m_usePerspectiveProjection) {
			meshes.m_basicVertexConstantBufferData.projection =
				XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY), m_aspect, m_nearZ, m_farZ);
		}
		else {
			meshes.m_basicVertexConstantBufferData.projection =
				XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
		}
		meshes.m_basicVertexConstantBufferData.projection = XMMatrixTranspose(meshes.m_basicVertexConstantBufferData.projection);

		// Constant를 CPU에서 GPU로 복사
		UpdateBuffer(m_d3dContext, meshes.m_basicVertexConstantBufferData, meshes.meshes[0]->vertexConstantBuffer);

		auto basicPixelData{ meshes.m_basicPixelConstantBufferData };

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
			meshes.meshes[0]->pixelConstantBuffer);

		// 노멀 벡터 그리기
		if (m_drawNormals && m_dirtyFlag) {

			UpdateBuffer(m_d3dContext, m_normalVertexConstantBufferData,
				m_normalLines->vertexConstantBuffer);

			m_dirtyFlag = false;

		}

		// 큐브매핑을 위한 ConstantBuffers
		meshes.m_basicVertexConstantBufferData.world = XMMatrixIdentity();
		// Transpose()도 생략 가능
		m_cubeMap.UpdateConstantBuffers(m_d3dDevice, m_d3dContext);

		UpdateBuffer(m_d3dContext, meshes.m_basicVertexConstantBufferData,
			m_cubeMap.cubeMesh->vertexConstantBuffer);

		meshes.m_basicPixelConstantBufferData.material.diffuse =
			CreateXMFLOAT3(m_materialDiffuse);
		meshes.m_basicPixelConstantBufferData.material.specular =
			CreateXMFLOAT3(m_materialSpecular);
		meshes.m_basicPixelConstantBufferData.material.fresnelR0=
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

		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // 검은색으로 클리어
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
		for (const auto& mesh : meshes.meshes) {
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
			ImGui::Checkbox("Use Texture", &meshes.m_basicPixelConstantBufferData.useTexture);
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
				&meshes.m_basicPixelConstantBufferData.material.shininess, 0.01f,
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
