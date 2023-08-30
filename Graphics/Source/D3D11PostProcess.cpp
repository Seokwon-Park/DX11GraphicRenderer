#include "D3D11PostProcess.h"

namespace graphics {
    void D3D11PostProcess::Initialize(ComPtr<ID3D11Device>& device,
        ComPtr<ID3D11DeviceContext>& context,
        const std::wstring vertexPrefix, const std::wstring pixelPrefix,
        int width, int height) {

        MeshData meshData = Geometry::MakePlane(1.f,Axis::y,Axis::z);

        m_mesh = std::make_shared<Mesh>();

        CreateVertexBuffer(device, meshData.vertices,
            m_mesh->vertexBuffer);
        m_mesh->m_indexCount = UINT(meshData.indices.size());
        D3D11Utilities::CreateIndexBuffer(device, meshData.indices,
            m_mesh->indexBuffer);

        std::vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
             D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
             D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3,
             D3D11_INPUT_PER_VERTEX_DATA, 0},
        };

        D3D11Utilities::CreateVertexShaderAndInputLayout(
            device, vertexPrefix + L"VS.hlsl", basicInputElements,
            m_vertexShader, m_inputLayout);

        D3D11Utilities::CreatePixelShader(device, pixelPrefix + L"PS.hlsl",
            m_pixelShader);

        // Texture sampler
        D3D11_SAMPLER_DESC sampDesc;
        ZeroMemory(&sampDesc, sizeof(sampDesc));
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

        // Create the Sample State
        device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf());

        // Create a rasterizer state
        D3D11_RASTERIZER_DESC rastDesc;
        ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
        rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
        rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
        rastDesc.FrontCounterClockwise = false;
        rastDesc.DepthClipEnable = false;

        device->CreateRasterizerState(&rastDesc,
            m_rasterizerState.GetAddressOf());

        // Set the viewport
        ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
        m_viewport.TopLeftX = 0;
        m_viewport.TopLeftY = 0;
        m_viewport.Width = float(width);
        m_viewport.Height = float(height);
        m_viewport.MinDepth = 0.0f;
        m_viewport.MaxDepth = 1.0f;

        ComPtr<ID3D11Texture2D> texture;

        D3D11_TEXTURE2D_DESC txtDesc;
        ZeroMemory(&txtDesc, sizeof(txtDesc));
        txtDesc.Width = width;
        txtDesc.Height = height;
        txtDesc.MipLevels = txtDesc.ArraySize = 1;
        txtDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //  이미지 처리용도
        txtDesc.SampleDesc.Count = 1;
        txtDesc.Usage = D3D11_USAGE_DEFAULT;
        txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE |
            D3D11_BIND_RENDER_TARGET |
            D3D11_BIND_UNORDERED_ACCESS;
        txtDesc.MiscFlags = 0;
        txtDesc.CPUAccessFlags = 0;

        D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
        viewDesc.Format = txtDesc.Format;
        viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        viewDesc.Texture2D.MipSlice = 0;

        device->CreateTexture2D(&txtDesc, NULL, texture.GetAddressOf());
        device->CreateRenderTargetView(texture.Get(), &viewDesc,
            m_renderTargetView.GetAddressOf());
        //renderTargetView로 접근, 텍스처에 직접 접근 X
        device->CreateShaderResourceView(texture.Get(), nullptr,
            m_shaderResourceView.GetAddressOf());

        m_pixelConstData.dx = 1.0f / width;
        m_pixelConstData.dy = 1.0f / height;

        CreateConstantBuffer(device, m_pixelConstData,
            m_mesh->pixelConstantBuffer);

        // 기본 렌더타겟
        this->SetRenderTargets({ m_renderTargetView });
    }
}