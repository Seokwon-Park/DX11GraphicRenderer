#include "D3D11Renderer.h"

namespace graphics
{
	D3D11Renderer::D3D11Renderer() : D3D11AppBase(), m_indexCount(0), m_constantBufferData() {}

	bool D3D11Renderer::Initialize()
	{
		if (!D3D11AppBase::Initialize())
			return false;

		MeshData data = Geometry::MakeCube(10, 11, 12, 1.f);

		m_mesh = std::make_shared<Mesh>();

		D3D11AppBase::CreateVertexBuffer(data.vertices, m_vertexBuffer);
		// 인덱스 버퍼 만들기
		m_indexCount = UINT(data.indices.size());
		D3D11AppBase::CreateIndexBuffer(data.indices, m_indexBuffer);

		// ConstantBuffer 만들기
		m_constantBufferData.model = XMMATRIX();
		m_constantBufferData.view = XMMATRIX();
		m_constantBufferData.projection = XMMATRIX();

		D3D11AppBase::CreateConstantBuffer(m_constantBufferData, m_mesh->vertexConstantBuffer);

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
			{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXTURE", }
			// 4 * 3 은 POSITION 4*3 다음 데이터부터 시작이라는 뜻
		};

		D3D11AppBase::CreateVertexShaderAndInputLayout(L"VertexShader.hlsl", inputElements,
			m_colorVertexShader, m_colorInputLayout);

		D3D11AppBase::CreatePixelShader(L"PixelShader.hlsl", m_colorPixelShader);

		return true;

	}


	void D3D11Renderer::Update(float dt)
	{
		using namespace DirectX;

		// 모델의 변환
		m_constantBufferData.model = XMMatrixScaling(m_modelScaling.x, m_modelScaling.y, m_modelScaling.z) *
			XMMatrixRotationY(m_modelRotation.y) *
			XMMatrixRotationX(m_modelRotation.x) *
			XMMatrixRotationZ(m_modelRotation.z) *
			XMMatrixTranslation(m_modelTranslation.x, m_modelTranslation.y, m_modelTranslation.z);
		m_constantBufferData.model = XMMatrixTranspose(m_constantBufferData.model);


		// 시점 변환
		m_constantBufferData.view =
			XMMatrixLookToLH(XMLoadFloat3(&m_viewEyePos), XMLoadFloat3(&m_viewEyeDir), XMLoadFloat3(&m_viewUp));
		m_constantBufferData.view =
			XMMatrixTranspose(m_constantBufferData.view);

		// 프로젝션
		if (m_usePerspectiveProjection) {
			m_constantBufferData.projection =
				XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY), m_aspect, m_nearZ, m_farZ);
		}
		else {
			m_constantBufferData.projection =
				XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
		}
		m_constantBufferData.projection = XMMatrixTranspose(m_constantBufferData.projection);

		// Constant를 CPU에서 GPU로 복사
		D3D11AppBase::UpdateBuffer(m_constantBufferData, m_constantBuffer);
	}

	void D3D11Renderer::Render()
	{
		// IA: Input-Assembler stage
		// VS: Vertex Shader
		// PS: Pixel Shader
		// RS: Rasterizer stage
		// OM: Output-Merger stage

		m_d3dContext->RSSetViewports(1, &m_screenViewport);

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

		m_d3dContext->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
		m_d3dContext->PSSetShader(m_colorPixelShader.Get(), 0, 0);

		m_d3dContext->RSSetState(m_rasterizerState.Get());

		// 버텍스/인덱스 버퍼 설정
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		m_d3dContext->IASetInputLayout(m_colorInputLayout.Get());
		m_d3dContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
		m_d3dContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_d3dContext->DrawIndexed(m_indexCount, 0, 0);
	}

	void D3D11Renderer::UpdateGUI()
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("General")) {
			ImGui::Checkbox("usePerspectiveProjection", &m_usePerspectiveProjection);

			ImGui::SliderFloat3("m_modelTranslation", &m_modelTranslation.x, -2.0f, 2.0f);
			ImGui::SliderFloat3("m_modelRotation(Rad)", &m_modelRotation.x, -3.14f, 3.14f);
			ImGui::SliderFloat3("m_modelScaling", &m_modelScaling.x, 0.1f, 2.0f);

			ImGui::SliderFloat3("m_viewEyePos", &m_viewEyePos.x, -4.0f, 4.0f);
			ImGui::SliderFloat3("m_viewEyeDir", &m_viewEyeDir.x, -4.0f, 4.0f);
			ImGui::SliderFloat3("m_viewUp", &m_viewUp.x, -2.0f, 2.0f);

			ImGui::SliderFloat("m_projFovAngleY(Deg)", &m_projFovAngleY, 10.0f, 180.0f);
			ImGui::SliderFloat("m_nearZ", &m_nearZ, 0.01f, m_farZ - 0.001f);
			ImGui::SliderFloat("m_farZ", &m_farZ, m_nearZ + 0.01f, 10.0f);
			ImGui::SliderFloat("m_aspect", &m_aspect, 1.0f, 3.0f);
			ImGui::TreePop();
		}
		
	}
}
