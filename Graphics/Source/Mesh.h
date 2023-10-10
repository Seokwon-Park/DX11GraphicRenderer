#pragma once
#include "CommonHeaders.h"
#include "D3D11CommonHeaders.h"

namespace graphics
{
    using Microsoft::WRL::ComPtr;

    // Vertex Struct
    struct Vertex {
        XMFLOAT3 position = XMFLOAT3();
        XMFLOAT3 normal = XMFLOAT3();
        XMFLOAT2 texcoord = XMFLOAT2();
    };

    // Mesh Data
    struct MeshData {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::string textureFilename;
    };

    struct Mesh {

        ComPtr<ID3D11Buffer> vertexBuffer;
        ComPtr<ID3D11Buffer> indexBuffer;

        ComPtr<ID3D11Buffer> vertexConstantBuffer;
        ComPtr<ID3D11Buffer> pixelConstantBuffer;

        ComPtr<ID3D11Texture2D> texture;
        ComPtr<ID3D11ShaderResourceView> textureResourceView;

        UINT m_indexCount = 0;
        UINT m_vertexCount = 0;
    };
}