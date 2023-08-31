#pragma once

#include "D3D11Utilities.h"
#include "D3D11Geometry.h"
#include "Mesh.h"


namespace graphics {

    class D3D11PostProcess {
    public:
        D3D11PostProcess(ComPtr<ID3D11Device>& device,
            ComPtr<ID3D11DeviceContext>& context,
            const std::wstring vertexPrefix, const std::wstring pixelPrefix,
            int width, int height) {
            Initialize(device, context, vertexPrefix, pixelPrefix, width, height);
        }

        void Initialize(ComPtr<ID3D11Device>& device,
            ComPtr<ID3D11DeviceContext>& context,
            const std::wstring vertexPrefix, const std::wstring pixelPrefix,
            int width, int height);


        void Render(ComPtr<ID3D11DeviceContext>& context);

        void UpdateConstantBuffers(ComPtr<ID3D11Device>& device,
            ComPtr<ID3D11DeviceContext>& context);

        void SetShaderResources(
            const std::vector<ComPtr<ID3D11ShaderResourceView>>& resources);

        void SetRenderTargets(
            const std::vector<ComPtr<ID3D11RenderTargetView>>& targets);

    public:
        ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
        ComPtr<ID3D11RenderTargetView> m_renderTargetView;

        struct SamplingPixelConstantData {
            float dx;
            float dy;
            float threshold;
            float strength;
            XMFLOAT4 options;
        };

        SamplingPixelConstantData m_pixelConstData;

    protected:
        std::shared_ptr<Mesh> m_mesh;

        ComPtr<ID3D11VertexShader> m_vertexShader;
        ComPtr<ID3D11PixelShader> m_pixelShader;
        ComPtr<ID3D11InputLayout> m_inputLayout;
        ComPtr<ID3D11SamplerState> m_samplerState;
        ComPtr<ID3D11RasterizerState> m_rasterizerState;

        D3D11_VIEWPORT m_viewport;

        // Do not delete pointers
        std::vector<ID3D11ShaderResourceView*> m_shaderResources;
        std::vector<ID3D11RenderTargetView*> m_renderTargets;
    };
} // namespace hlab