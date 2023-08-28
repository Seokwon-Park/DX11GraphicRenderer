#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem>
#include "Mesh.h"

namespace graphics {
    class AssimpLoader {
    public:
        void Load(std::string filename);

        void ProcessNode(aiNode* node, const aiScene* scene,
            XMMATRIX tr);

        MeshData ProcessMesh(aiMesh* mesh, const aiScene* scene);

    public:
        std::vector<MeshData> meshes;
    };
} // namespace hlab