#pragma once
#include "CommonHeaders.h"


namespace graphics {
    using namespace DirectX;


    // Vertex Struct
    struct Vertex {
        XMFLOAT3 position;
        XMFLOAT3 normal;
        XMFLOAT2 texcoord;
    };

    // Mesh Data
    struct MeshData {
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices; // uint32·Î º¯°æ
        std::string textureFilename;
    };

    struct Mesh {
        ComPtr<ID3D11Buffer> vertexBuffer;
        ComPtr<ID3D11Buffer> indexBuffer;
        ComPtr<ID3D11Buffer> vertexConstantBuffer;
        ComPtr<ID3D11Buffer> pixelConstantBuffer;

        UINT m_indexCount = 0;
    };

    class Geometry {
    public:
        //static std::vector<MeshData> ReadFromFile(std::string basePath,
        //    std::string filename);

        static MeshData MakeSquare();
        static MeshData MakeCube(float width, float height, float depth, float scale);
        //static MeshData MakeCylinder(const float bottomRadius,
        //    const float topRadius, float height,
        //    int numSlices);
        //static MeshData MakeSphere(const float radius, const int numSlices,
        //    const int numStacks);
        //static MeshData MakeTetrahedron();
        //static MeshData MakeIcosahedron();
        //static MeshData SubdivideToSphere(const float radius, MeshData meshData);
    };
} // namespace hlab
