#include "D3D11Renderer.h"
#include <tuple>
namespace graphics
{

    auto MakeBox() {
        std::vector<XMFLOAT3> positions;
        std::vector<XMFLOAT3> colors;
        std::vector<XMFLOAT3> normals;

        const float scale = 1.0f;

        // 윗면
        positions.push_back(XMFLOAT3(-1.0f, 1.0f, -1.0f));
        positions.push_back(XMFLOAT3(-1.0f, 1.0f, 1.0f));
        positions.push_back(XMFLOAT3(1.0f, 1.0f, 1.0f) );
        positions.push_back(XMFLOAT3(1.0f, 1.0f, -1.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));

        // 아랫면
        positions.push_back(XMFLOAT3(-1.0f, -1.0f, 1.0f)) ;
        positions.push_back(XMFLOAT3(-1.0f, -1.0f, -1.0f));
        positions.push_back(XMFLOAT3(1.0f, -1.0f, -1.0f) );
        positions.push_back(XMFLOAT3(1.0f, -1.0f, 1.0f) );
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));

        // 앞면
        positions.push_back(XMFLOAT3(-1.0f, -1.0f, -1.0f) );
        positions.push_back(XMFLOAT3(-1.0f, 1.0f, -1.0f) );
        positions.push_back(XMFLOAT3(1.0f, 1.0f, -1.0f) );
        positions.push_back(XMFLOAT3(1.0f, -1.0f, -1.0f));
        colors.push_back(XMFLOAT3(1.0f, 1.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 1.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 1.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 1.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));
        normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));
        normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));
        normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));


        // 뒷면
        positions.push_back(XMFLOAT3(-1.0f, 1.0f, 1.0f));
        positions.push_back(XMFLOAT3(-1.0f, -1.0f, 1.0f));
        positions.push_back(XMFLOAT3(1.0f, -1.0f, 1.0f));
        positions.push_back(XMFLOAT3(1.0f, 1.0f, 1.0f) );
        colors.push_back(XMFLOAT3(1.0f, 1.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 1.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 1.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 1.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
        normals.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
        normals.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
        normals.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));

        // 왼쪽
        positions.push_back(XMFLOAT3(-1.0f, -1.0f, -1.0f));
        positions.push_back(XMFLOAT3(-1.0f, -1.0f, 1.0f) );
        positions.push_back(XMFLOAT3(-1.0f, 1.0f, 1.0f) );
        positions.push_back(XMFLOAT3(-1.0f, 1.0f, -1.0f) );
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 1.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 1.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 1.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 1.0f));
        normals.push_back(XMFLOAT3(-1.0f, 0.0f, 0.0f));
        normals.push_back(XMFLOAT3(-1.0f, 0.0f, 0.0f));
        normals.push_back(XMFLOAT3(-1.0f, 0.0f, 0.0f));
        normals.push_back(XMFLOAT3(-1.0f, 0.0f, 0.0f));

        // 오른쪽
        positions.push_back(XMFLOAT3(1.0f, -1.0f, -1.0f) );
        positions.push_back(XMFLOAT3(1.0f, 1.0f, -1.0f) );
        positions.push_back(XMFLOAT3(1.0f, 1.0f, 1.0f) );
        positions.push_back(XMFLOAT3(1.0f, -1.0f, 1.0f) );
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 1.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 1.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 1.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 1.0f));
        normals.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
        normals.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
        normals.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
        normals.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));

        std::vector<Vertex> vertices;
        for (size_t i = 0; i < positions.size(); i++) {
            Vertex v;
            v.position = positions[i];
            v.color = colors[i];
            vertices.push_back(v);
        }

        std::vector<uint16_t> indices = {
            0,  1,  2,  0,  2,  3,  // 윗면
            4,  5,  6,  4,  6,  7,
            8,  9,  10, 8,  10, 11,
            12, 13, 14, 12, 14, 15,
            16, 17, 18, 16, 18, 19,
            20, 21, 22, 20, 22, 23,
        };

        return std::tuple{ vertices, indices };
    }

	D3D11Renderer::D3D11Renderer() : D3D11AppBase(), m_indexCount(0) {}

	bool D3D11Renderer::Initialize()
	{
		if (!D3D11AppBase::Initialize())
			return false;

        auto [vertices, indices] = MakeBox();

        D3D11AppBase::CreateIndexBuffer(indices, m_indexBuffer);

        // ConstantBuffer 만들기
        m_constantBufferData.model = XMMATRIX();
        m_constantBufferData.view = XMMATRIX();
        m_constantBufferData.projection = XMMATRIX();

        D3D11AppBase::CreateConstantBuffer(m_constantBufferData, m_constantBuffer);

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
            // 4 * 3 은 POSITION 4*3 다음 데이터부터 시작이라는 뜻
        };

        D3D11AppBase::CreateVertexShaderAndInputLayout(L"VertexShader.hlsl", inputElements,
            m_colorVertexShader, m_colorInputLayout);

        D3D11AppBase::CreatePixelShader(L"PixelShader.hlsl", m_colorPixelShader);

        return true;

	}


	void D3D11Renderer::UpdateGUI()
	{
		ImGui::Checkbox("usePerspectiveProjection", &m_usePerspectiveProjection);
	}

	void D3D11Renderer::Update(float dt)
	{
        static float rot = 0.0f;
        rot += dt;

        // 모델의 변환
        m_constantBufferData.model = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(rot) *
            XMMatrixTranslation(0.0f, -0.3f, 1.0f);
        m_constantBufferData.model = XMMatrixTranspose(m_constantBufferData.model);

        using namespace DirectX;

        // 시점 변환
        m_constantBufferData.view =
            XMMatrixLookAtLH({ 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f });
        m_constantBufferData.view = XMMatrixTranspose(m_constantBufferData.view);

        // 프로젝션
        const float aspect = D3D11AppBase::GetAspectRatio();
        if (m_usePerspectiveProjection) {
            const float fovAngleY = 70.0f * XM_PI / 180.0f;
            m_constantBufferData.projection =
                XMMatrixPerspectiveFovLH(fovAngleY, aspect, 0.01f, 100.0f);
        }
        else {
            m_constantBufferData.projection =
                XMMatrixOrthographicOffCenterLH(-aspect, aspect, -1.0f, 1.0f, 0.1f, 10.0f);
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
}