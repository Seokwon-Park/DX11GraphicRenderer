#pragma once
#include "CommonHeaders.h"


namespace graphics {
    using namespace DirectX;
    using namespace Microsoft::WRL;

    // Vertex Struct
    struct Vertex {
        XMFLOAT3 position;
        XMFLOAT3 normal;
        XMFLOAT2 texcoord;
    };

    struct Axis
    {
        enum :int
        {
            x,
            y,
            z = 2
        };
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
    };

    class Geometry {
    public:
        //static std::vector<MeshData> ReadFromFile(std::string basePath,
        //    std::string filename);
        static MeshData MakeSquare(float length, int axis1 = Axis::x, int axis2 = Axis::y);
        static MeshData MakeCube(float width, float height, float depth, float scale);
        static MeshData MakeGrid(const float width, const float height,
            const int numSlices, const int numStacks);
        static MeshData MakeCylinder(const float bottomRadius, const float topRadius, float height, int numSlices, int numStacks);
        static MeshData MakeSphere(const float radius, const int numSlices,
            const int numStacks);

        //static MeshData MakeTetrahedron();
        //static MeshData MakeIcosahedron();
        static MeshData SubdivideToSphere(const float radius, MeshData meshData);
        static std::vector<MeshData> ReadModelFromFile(std::string filename);
    };
} // namespace hlab
