#pragma once

#include "D3D11CommonHeaders.h"
#include "AssimpLoader.h"

#include "Mesh.h"


namespace graphics {

    struct Axis
    {
        enum :int
        {
            x,
            y,
            z = 2
        };
    };

    class Geometry {
    public:
        //static std::vector<MeshData> ReadFromFile(std::string basePath,
        //    std::string filename);
        static MeshData MakePlane(float length, int axis1 = Axis::x, int axis2 = Axis::y);
        static MeshData MakeCube(float width, float height, float depth);
        static MeshData MakeGrid(const float width, const float height,
            const int numSlices, const int numStacks);
        static MeshData MakeCylinder(const float bottomRadius, const float topRadius, float height, int numSlices, int numStacks);
        static MeshData MakeSphere(const float radius, const int numSlices,
            const int numStacks);

        static MeshData MakeTetrahedron();
        static MeshData MakeIcosahedron();
        static MeshData SubdivideToSphere(const float radius, MeshData meshData);
        static std::vector<MeshData> ReadModelFromFile(std::string filename);
    };
} // namespace graphics
