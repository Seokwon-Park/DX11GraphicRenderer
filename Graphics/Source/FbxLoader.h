#pragma once

#include <fbxsdk.h>
#include "CommonHeaders.h"

#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.2\\lib\\vs2019\\x64\\debug\\libfbxsdk-md.lib")
#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.2\\lib\\vs2019\\x64\\debug\\libxml2-md.lib")
#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.2\\lib\\vs2019\\x64\\debug\\zlib-md.lib")

namespace graphics
{
	static class FbxLoader
	{
	public:
		FbxLoader();
		bool Initialize();
		void release();
		void ProcessNode(FbxNode* node, const FbxScene* scene);
		MeshData ProcessMesh(FbxMesh* mesh, const FbxScene* scene);
		std::vector<MeshData> meshes;
	protected:
	private:
		FbxManager* m_fbxManager;
		FbxImporter* m_fbxImporter;
	};
}