#include "FbxLoader.h"

namespace graphics
{
	FbxLoader::FbxLoader() {};

	bool FbxLoader::Initialize()
	{
		//가져오는 순서 Importer 초기화 - Importer에서 Scene 가져오기 - 이후 모델 불러올 수 있다.
		const char* lFilename = "d:/zelda.fbx";

		// Initialize the SDK manager. This object handles memory management.
		m_fbxManager = FbxManager::Create();

		// Create the IO settings object.
		FbxIOSettings* ios = FbxIOSettings::Create(m_fbxManager, IOSROOT);
		m_fbxManager->SetIOSettings(ios);

		// Create an importer using the SDK manager.
		m_fbxImporter = FbxImporter::Create(m_fbxManager, "");

		// Use the first argument as the filename for the importer.
		if (!m_fbxImporter->Initialize(lFilename, -1, m_fbxManager->GetIOSettings())) {
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", m_fbxImporter->GetStatus().GetErrorString());
			exit(-1);
		}

		// Create a new scene so that it can be populated by the imported file.
		FbxScene* lScene = FbxScene::Create(m_fbxManager, "myScene");
		FbxAxisSystem::OpenGL.ConvertScene(lScene);
		// Import the contents of the file into the scene.
		m_fbxImporter->Import(lScene);
		// The file is imported, so get rid of the importer.
		m_fbxImporter->Destroy();

		FbxNode* lRootNode = lScene->GetRootNode();
		if (lRootNode) {
			ProcessNode(lRootNode, lScene);
		}
		// Destroy the SDK manager and all the other objects it was handling.
		m_fbxManager->Destroy();

		return true;
	}

	void FbxLoader::release()
	{

	}

	void FbxLoader::ProcessNode(FbxNode* node, const FbxScene* scene)
	{
		const int num_attributes{ node->GetNodeAttributeCount() };
		for (int i{ 0 }; i < num_attributes; i++)
		{
			FbxNodeAttribute* attribute{ node->GetNodeAttributeByIndex(i) };
			const FbxNodeAttribute::EType attribute_type{ attribute->GetAttributeType() };
			if (attribute_type == FbxNodeAttribute::eMesh)
			{
				FbxMesh* mesh = (FbxMesh*)attribute;
				FbxGeometryConverter gc{ m_fbxManager };
				mesh = (FbxMesh*)gc.Triangulate(mesh, true);
				auto newMesh = ProcessMesh(mesh, scene);

				meshes.emplace_back(newMesh);
			}
		}
		if (const int numChild = node->GetChildCount())
		{
			for (int i = 0; i < numChild; i++) {
				ProcessNode(node->GetChild(i), scene);
			}
		}

		// Normalize vertices
		XMFLOAT3 vmin(1000, 1000, 1000);
		XMFLOAT3 vmax(-1000, -1000, -1000);
		for (auto& mesh : meshes) {
			for (auto& v : mesh.vertices) {
				vmin.x = XMMin(vmin.x, v.position.x);
				vmin.y = XMMin(vmin.y, v.position.y);
				vmin.z = XMMin(vmin.z, v.position.z);
				vmax.x = XMMax(vmax.x, v.position.x);
				vmax.y = XMMax(vmax.y, v.position.y);
				vmax.z = XMMax(vmax.z, v.position.z);
			}
		}

		float dx = vmax.x - vmin.x, dy = vmax.y - vmin.y, dz = vmax.z - vmin.z;
		float dl = XMMax(XMMax(dx, dy), dz);
		float cx = (vmax.x + vmin.x) * 0.5f, cy = (vmax.y + vmin.y) * 0.5f,
			cz = (vmax.z + vmin.z) * 0.5f;

		for (auto& mesh : meshes) {
			for (auto& v : mesh.vertices) {
				v.position.x = (v.position.x - cx) / dl;
				v.position.y = (v.position.y - cy) / dl;
				v.position.z = (v.position.z - cz) / dl;
			}
		}

	}


	MeshData FbxLoader::ProcessMesh(FbxMesh* mesh, const FbxScene* scene) {

		// Data to fill
		std::vector<Vertex> vertices;
		std::vector<uint16_t> indices;

		FbxVector4* controlPoints = mesh->GetControlPoints();
		int vertexCount = mesh->GetControlPointsCount();
		int polygonCount = mesh->GetPolygonCount();

		for (int i = 0; i < vertexCount; i++)
		{


			for (int j = 0; j < 3; ++j) {


				//XMStoreFloat3(&vertex.normal, XMVectorSubtract(XMLoadFloat3(&vertex.position), XMVectorSet(0.f, 0.f, 0.f, 0.f)));

			}
		}


		for (int i = 0; i < polygonCount; ++i) {
			//int gpsize = mesh->GetPolygonSize(i) - 2; // if 4 = 2 tri, 3 = 1 tri
			Vertex vertex;
			//int vertexIndex[3] = { 0, 1, 2 };
			int p1 = mesh->GetPolygonVertex(i, 0);
			int p2 = mesh->GetPolygonVertex(i, 1);
			int p3 = mesh->GetPolygonVertex(i, 2);
			FbxVector4 controlPoint1 = controlPoints[p1];
			FbxVector4 controlPoint2 = controlPoints[p2];
			FbxVector4 controlPoint3 = controlPoints[p3];

			FbxVector4 side1 = controlPoint2 - controlPoint1;
			FbxVector4 side2 = controlPoint3 - controlPoint1;

			XMVECTOR a = XMVectorSet(side1[0], side1[1], side1[2], side1[3]);
			XMVECTOR b = XMVectorSet(side2[0], side2[1], side2[2], side2[3]);
			XMFLOAT3 result;
			XMStoreFloat3(&result, XMVector3Normalize(XMVector3Cross(a, b)));

			//FbxArray<FbxVector4> normalElement;
			//mesh->GetPolygonVertexNormals(normalElement);
			FbxGeometryElementNormal* normalElement = mesh->GetElementNormal(0);

			int sibalcpp = mesh->GetElementNormalCount() * 3;
			//for (int m = 0; m < 24; m++)
			//{
			//	FbxVector4 normal = normalElement->GetDirectArray().GetAt(m);
			//	std::cout << m <<" " << normal[0] << " " << normal[1] << " " << normal[2] << "\n";
			//}

			FbxVector4 controlPoint = controlPoints[p1];
			vertex.position.x = controlPoint[0];
			vertex.position.y = controlPoint[1];
			vertex.position.z = controlPoint[2];		
			//vertex.normal = result;

			FbxVector4 normal = normalElement->GetDirectArray().GetAt(p1*3);
			vertex.normal = XMFLOAT3(normal[0], normal[1], normal[2]);
			vertices.push_back(vertex);

			controlPoint = controlPoints[p2];
			vertex.position.x = controlPoint[0];
			vertex.position.y = controlPoint[1];
			vertex.position.z = controlPoint[2];		
			//vertex.normal = result;
			vertex.normal = XMFLOAT3(normal[0], normal[1], normal[2]);
			vertices.push_back(vertex);

			controlPoint = controlPoints[p3];
			vertex.position.x = controlPoint[0];
			vertex.position.y = controlPoint[1];
			vertex.position.z = controlPoint[2];
			//vertex.normal = result;
			vertex.normal = XMFLOAT3(normal[0], normal[1], normal[2]);
			vertices.push_back(vertex);

			indices.push_back(i*3);
			indices.push_back(i * 3+1);
			indices.push_back(i * 3+2);

		}

		MeshData newMesh;
		newMesh.vertices = vertices;
		newMesh.indices = indices;

		return newMesh;
	}

}
