#include "D3D11Renderer.h"

namespace graphics
{
	D3D11Renderer::D3D11Renderer() : D3D11AppBase(), m_vertexConstantBufferData() {}

	bool D3D11Renderer::Initialize()
	{
		if (!D3D11AppBase::Initialize())
			return false;

		D3D11AppBase::CreateTexture("D:\\Graphics\\wall.jpg", m_texture,
			m_textureResourceView);


		// Texture sampler 만들기
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		// Create the Sample State
		m_d3dDevice->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf());

		MeshData meshData = Geometry::MakeCube(2.f, 2.f, 2.f, 1.f);
		//MeshData meshData = Geometry::MakeSquare();

		m_mesh = std::make_shared<Mesh>();

		D3D11AppBase::CreateVertexBuffer(meshData.vertices, m_mesh->vertexBuffer);
		// 인덱스 버퍼 만들기
		m_mesh->m_indexCount= UINT(meshData.indices.size());
		D3D11AppBase::CreateIndexBuffer(meshData.indices, m_mesh->indexBuffer);

		// ConstantBuffer 만들기
		m_vertexConstantBufferData.world = XMMATRIX();
		m_vertexConstantBufferData.view = XMMATRIX();
		m_vertexConstantBufferData.projection = XMMATRIX();

		D3D11AppBase::CreateConstantBuffer(m_vertexConstantBufferData, m_mesh->vertexConstantBuffer);

		D3D11AppBase::CreateConstantBuffer(m_pixelConstantBufferData, m_mesh->pixelConstantBuffer);

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
			// 4 * 3 은 POSITION 4*3 다음 데이터부터 시작이라는 뜻
		};

		D3D11AppBase::CreateVertexShaderAndInputLayout(L"BasicVertexShader.hlsl", inputElements, m_colorVertexShader,
			m_colorInputLayout);

		D3D11AppBase::CreatePixelShader(L"BasicPixelShader.hlsl", m_colorPixelShader);

		return true;
	}


	void D3D11Renderer::Update(float dt)
	{
		using namespace DirectX;

		// 모델의 변환
		m_vertexConstantBufferData.world = XMMatrixScaling(m_modelScaling.x, m_modelScaling.y, m_modelScaling.z) *
			XMMatrixRotationY(m_modelRotation.y) *
			XMMatrixRotationX(m_modelRotation.x) *
			XMMatrixRotationZ(m_modelRotation.z) *
			XMMatrixTranslation(m_modelTranslation.x, m_modelTranslation.y, m_modelTranslation.z);
		m_vertexConstantBufferData.world = XMMatrixTranspose(m_vertexConstantBufferData.world);

		// 역행렬의 전치 행렬
		m_vertexConstantBufferData.invTranspose = m_vertexConstantBufferData.world;
		m_vertexConstantBufferData.invTranspose *= XMMatrixTranslation(m_modelTranslation.x, m_modelTranslation.y, m_modelTranslation.z);
		m_vertexConstantBufferData.invTranspose =
			XMMatrixInverse(nullptr,XMMatrixTranspose(m_vertexConstantBufferData.invTranspose));


		// 시점 변환
		m_vertexConstantBufferData.view =
			XMMatrixRotationY(m_viewRot) *
			XMMatrixTranslation(0.0f, 0.0f, 2.0f);

		
		XMStoreFloat3(&m_pixelConstantBufferData.eyeWorld, XMVector3Transform(
			XMVectorZero(), XMMatrixInverse(nullptr, m_vertexConstantBufferData.view)));

		m_vertexConstantBufferData.view =
			XMMatrixTranspose(m_vertexConstantBufferData.view);

		// 프로젝션
		m_aspect = D3D11AppBase::GetAspectRatio();
		if (m_usePerspectiveProjection) {
			m_vertexConstantBufferData.projection =
				XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY), m_aspect, m_nearZ, m_farZ);
		}
		else {
			m_vertexConstantBufferData.projection =
				XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
		}
		m_vertexConstantBufferData.projection = XMMatrixTranspose(m_vertexConstantBufferData.projection);

		// Constant를 CPU에서 GPU로 복사
		D3D11AppBase::UpdateBuffer(m_vertexConstantBufferData, m_mesh->vertexConstantBuffer);

		m_pixelConstantBufferData.material.diffuse = XMFLOAT3(m_materialDiffuse, m_materialDiffuse, m_materialDiffuse);
		m_pixelConstantBufferData.material.specular = XMFLOAT3(m_materialSpecular, m_materialSpecular, m_materialSpecular );

		// 여러 개 조명 사용 예시
		for (int i = 0; i < MAX_LIGHTS; i++) {
			// 다른 조명 끄기
			if (i != m_lightType) {
				XMVECTOR strength = XMLoadFloat3(&m_pixelConstantBufferData.lights[i].strength);
				XMStoreFloat3(&m_pixelConstantBufferData.lights[i].strength, XMVectorScale(strength, 0.0f));
			}
			else {
				m_pixelConstantBufferData.lights[i] = m_lightFromGUI;
			}
		}

		D3D11AppBase::UpdateBuffer(m_pixelConstantBufferData,
			m_mesh->pixelConstantBuffer);
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
		// m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);
		m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

		m_d3dContext->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

		// 어떤 쉐이더를 사용할지 설정
		m_d3dContext->VSSetShader(m_colorVertexShader.Get(), 0, 0);

		/* 경우에 따라서는 포인터의 배열을 넣어줄 수도 있습니다.
		ID3D11Buffer *pptr[1] = {
			m_constantBuffer.Get(),
		};
		m_context->VSSetConstantBuffers(0, 1, pptr); */

		m_d3dContext->VSSetConstantBuffers(0, 1, m_mesh->vertexConstantBuffer.GetAddressOf());

		ID3D11ShaderResourceView* pixelResources[1] = { m_textureResourceView.Get() };
		//좀 괜찮은 쉐이더를 짤때는 텍스처 여러장을 사용하기 때문에 배열 형태로
		m_d3dContext->PSSetShaderResources(0, 1, pixelResources);
		m_d3dContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

		m_d3dContext->PSSetConstantBuffers(0, 1, m_mesh->pixelConstantBuffer.GetAddressOf());
		m_d3dContext->PSSetShader(m_colorPixelShader.Get(), 0, 0);



		m_d3dContext->RSSetState(m_rasterizerState.Get());

		// 버텍스/인덱스 버퍼 설정
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		m_d3dContext->IASetInputLayout(m_colorInputLayout.Get());
		m_d3dContext->IASetVertexBuffers(0, 1, m_mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		m_d3dContext->IASetIndexBuffer(m_mesh->indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_d3dContext->DrawIndexed(m_mesh->m_indexCount, 0, 0);
	}

	void D3D11Renderer::UpdateGUI()
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("General")) {
			ImGui::Checkbox("Use Texture", &m_pixelConstantBufferData.useTexture);
			ImGui::Checkbox("usePerspectiveProjection", &m_usePerspectiveProjection);

			ImGui::SliderFloat3("m_modelTranslation", &m_modelTranslation.x, -2.0f, 2.0f);
			ImGui::SliderFloat3("m_modelRotation(Rad)", &m_modelRotation.x, -3.14f, 3.14f);
			ImGui::SliderFloat3("m_modelScaling", &m_modelScaling.x, 0.1f, 2.0f);

			ImGui::SliderFloat("m_viewRot", &m_viewRot, -3.14f, 3.14f);

			//ImGui::SliderFloat3("m_viewEyePos", &m_viewEyePos.x, -4.0f, 4.0f);
			//ImGui::SliderFloat3("m_viewEyeDir", &m_viewEyeDir.x, -4.0f, 4.0f);
			//ImGui::SliderFloat3("m_viewUp", &m_viewUp.x, -2.0f, 2.0f);

			ImGui::SliderFloat("m_projFovAngleY(Deg)", &m_projFovAngleY, 10.0f, 180.0f);
			ImGui::SliderFloat("m_nearZ", &m_nearZ, 0.01f, m_farZ - 0.001f);
			ImGui::SliderFloat("m_farZ", &m_farZ, m_nearZ + 0.01f, 10.0f);
			ImGui::SliderFloat("m_aspect", &m_aspect, 1.0f, 3.0f);
			ImGui::TreePop();
		}

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Lights")) {
			ImGui::Checkbox("Use BlinnPhong", &m_pixelConstantBufferData.useBlinnPhong);
			ImGui::SliderFloat("Material Shininess",
				&m_pixelConstantBufferData.material.shininess, 1.0f,
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
