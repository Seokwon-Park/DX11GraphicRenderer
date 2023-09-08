#pragma once

#include "D3D11CommonHeaders.h"
#include "CommonHeaders.h"

namespace graphics {
    struct BillboardPointsConstantData {
        XMFLOAT3 eyeWorld;
        float width;
        XMMATRIX model; // Vertex shader
        XMMATRIX view;  // Vertex shader
        XMMATRIX proj;  // Pixel shader
    };

    static_assert((sizeof(BillboardPointsConstantData) % 16) == 0,
        "Constant Buffer size must be 16-byte aligned");

    class BillboardPoints {
    public:
        void Initialize(ComPtr<ID3D11Device>& device,
            const std::vector<XMFLOAT4>& points);

        void Render(ComPtr<ID3D11DeviceContext>& context);

    public:
        BillboardPointsConstantData m_constantData;

        // 편의상 ConstantBuffer를 하나만 사용
        ComPtr<ID3D11Buffer> m_constantBuffer;

    protected:
        ComPtr<ID3D11Buffer> m_vertexBuffer;
        ComPtr<ID3D11SamplerState> m_samplerState;
        ComPtr<ID3D11VertexShader> m_vertexShader;
        ComPtr<ID3D11GeometryShader> m_geometryShader;
        ComPtr<ID3D11PixelShader> m_pixelShader;
        ComPtr<ID3D11InputLayout> m_inputLayout;

        ComPtr<ID3D11Texture2D> m_texArray;
        ComPtr<ID3D11ShaderResourceView> m_texArraySRV;

        uint32_t m_indexCount = 0;
    };
} // namespace graphics