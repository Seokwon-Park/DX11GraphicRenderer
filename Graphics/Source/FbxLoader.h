#pragma once

#include "CommonHeaders.h"
#if USE_FBX_SDK
#include <fbxsdk.h>
#if _DEBUG
#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.2\\lib\\vs2019\\x64\\debug\\libfbxsdk-md.lib")
#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.2\\lib\\vs2019\\x64\\debug\\libxml2-md.lib")
#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.2\\lib\\vs2019\\x64\\debug\\zlib-md.lib")
#else
#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.2\\lib\\vs2019\\x64\\release\\libfbxsdk-md.lib")
#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.2\\lib\\vs2019\\x64\\release\\libxml2-md.lib")
#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.2\\lib\\vs2019\\x64\\release\\zlib-md.lib")
#endif

namespace graphics
{	
	class FbxLoader
	{
	public:
		FbxLoader();
		void Load(const char* filename);
		void ProcessNode(FbxNode* node, XMMATRIX tr);
		MeshData ProcessMesh(FbxMesh* mesh);
		std::vector<MeshData> meshes;
	protected:
	private:
		FbxManager* m_fbxManager;
		FbxImporter* m_fbxImporter;
	};
}
#endif