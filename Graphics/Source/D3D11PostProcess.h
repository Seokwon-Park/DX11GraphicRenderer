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

        void UpdateConstantBuffers(ComPtr<ID3D11Device>& device,
            ComPtr<ID3D11DeviceContext>& context) {

            UpdateBuffer(context, m_pixelConstData,
                m_mesh->pixelConstantBuffer);
        }

        void Render(ComPtr<ID3D11DeviceContext>& context) {

            assert(m_shaderResources.size() > 0);
            assert(m_renderTargets.size() > 0);

            // 어디에 렌더링 할지를 지정
            context->OMSetRenderTargets(UINT(m_renderTargets.size()),
                m_renderTargets.data(), nullptr);
            //float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
            //context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
            context->RSSetState(m_rasterizerState.Get());
            context->RSSetViewports(1, &m_viewport);

            UINT stride = sizeof(Vertex);
            UINT offset = 0;

            context->IASetInputLayout(m_inputLayout.Get());
            context->IASetVertexBuffers(0, 1, m_mesh->vertexBuffer.GetAddressOf(),
                &stride, &offset);
            context->IASetIndexBuffer(m_mesh->indexBuffer.Get(),
                DXGI_FORMAT_R32_UINT, 0);
            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            context->VSSetShader(m_vertexShader.Get(), 0, 0);
            context->PSSetShader(m_pixelShader.Get(), 0, 0);
            context->PSSetShaderResources(0, UINT(m_shaderResources.size()),
                m_shaderResources.data());
            context->PSSetConstantBuffers(
                0, 1, m_mesh->pixelConstantBuffer.GetAddressOf());
            context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
            context->DrawIndexed(m_mesh->m_indexCount, 0, 0);
        }

        void SetShaderResources(
            const std::vector<ComPtr<ID3D11ShaderResourceView>>& resources) {

            m_shaderResources.clear();
            for (const auto& res : resources) {
                m_shaderResources.push_back(res.Get());
            }
        }

        void SetRenderTargets(
            const std::vector<ComPtr<ID3D11RenderTargetView>>& targets) {

            m_renderTargets.clear();
            for (const auto& tar : targets) {
                m_renderTargets.push_back(tar.Get());
            }
        }

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