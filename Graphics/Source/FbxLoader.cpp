#include "FbxLoader.h"

#if USE_FBX_SDK
namespace graphics
{
	FbxLoader::FbxLoader() {};

	namespace
	{

	}//anonymous namespace

	void FbxLoader::Load(const char* filename)
	{
		// Initialize the SDK manager. This object handles memory management.
		m_fbxManager = FbxManager::Create();

		// Create the IO settings object.
		FbxIOSettings* ios = FbxIOSettings::Create(m_fbxManager, IOSROOT);
		m_fbxManager->SetIOSettings(ios);

		// Create an importer using the SDK manager.
		m_fbxImporter = FbxImporter::Create(m_fbxManager, "");

		// Use the first argument as the filename for the importer.
		if (!m_fbxImporter->Initialize(filename , -1, m_fbxManager->GetIOSettings())) {
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", m_fbxImporter->GetStatus().GetErrorString());
			exit(-1);
		}

		// Create a new scene so that it can be populated by the imported file.
		FbxScene* lScene = FbxScene::Create(m_fbxManager, "myScene");
		lScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::eDirectX);
		// Import the contents of the file into the scene.
		m_fbxImporter->Import(lScene);
		// The file is imported, so get rid of the importer.
		m_fbxImporter->Destroy();

		FbxNode* lRootNode = lScene->GetRootNode();
		if (lRootNode) {
			XMMATRIX tr = XMMatrixIdentity();
			ProcessNode(lRootNode, tr);
		}
		// Destroy the SDK manager and all the other objects it was handling.
		m_fbxManager->Destroy();
	}


	void FbxLoader::ProcessNode(FbxNode* node, XMMATRIX tr)
	{
		XMMATRIX m;
		double* temp= node->EvaluateLocalTransform();
		float* mTemp = &m.r[0].m128_f32[0];

		for (int t = 0; t < 16; t++) {
			mTemp[t] = float(temp[t]);
		}

		m = XMMatrixTranspose(m) * tr;

		const int num_attributes{ node->GetNodeAttributeCount() };
		for (int i{ 0 }; i < num_attributes; i++)
		{
			FbxNodeAttribute* attribute{ node->GetNodeAttributeByIndex(i) };
			const FbxNodeAttribute::EType attribute_type{ attribute->GetAttributeType() };
			if (attribute_type == FbxNodeAttribute::eMesh)
			{
				FbxMesh* mesh = node->GetMesh();

				FbxGeometryConverter gc{ m_fbxManager };
				mesh = (FbxMesh*)gc.Triangulate(mesh, true);

				auto newMesh = ProcessMesh(mesh);

				for (auto& v : newMesh.vertices) {
					XMStoreFloat3(&v.position, XMVector3Transform(XMLoadFloat3(&v.position), m));
				}

				meshes.emplace_back(newMesh);
			}
		}
		if (const int numChild = node->GetChildCount())
		{
			for (int i = 0; i < numChild; i++) {
				ProcessNode(node->GetChild(i), m);
			}
		}
	}


	MeshData FbxLoader::ProcessMesh(FbxMesh* mesh) {

		// Data to fill
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		FbxVector4* controlPoints = mesh->GetControlPoints();
		int vertexCount = mesh->GetControlPointsCount();
		int polygonCount = mesh->GetPolygonCount();


		for (int i = 0; i < mesh->GetPolygonCount(); i++)
		{
			int faceSize = mesh->GetPolygonSize(i);
			for (int j = 0; j < faceSize; j++) {
				int iControlPointIndex = mesh->GetPolygonVertex(i, j);

				Vertex vertex;
				vertex.position.x = (float)controlPoints[iControlPointIndex].mData[0];
				vertex.position.y = (float)controlPoints[iControlPointIndex].mData[1];
				vertex.position.z = (float)controlPoints[iControlPointIndex].mData[2];
				
				FbxGeometryElementNormal* normalElement = mesh->GetElementNormal(0);
				for (int k = 0; k < 100; k++)
				{
					std::cout << normalElement->GetDirectArray().GetAt(k)[0]
						<< " " << normalElement->GetDirectArray().GetAt(k)[1]
						<< " " << normalElement->GetDirectArray().GetAt(k)[2] << "\n";
				}
				vertex.normal.x = normalElement->GetDirectArray().GetAt(i)[0];
				vertex.normal.y = normalElement->GetDirectArray().GetAt(i)[1];
				vertex.normal.z = -normalElement->GetDirectArray().GetAt(i)[2];

				vertices.push_back(vertex);
				indices.push_back(i*3+j);
			}
		}
		//for (int i = 0; i < polygonCount; ++i) {
		//	//int gpsize = mesh->GetPolygonSize(i) - 2; // if 4 = 2 tri, 3 = 1 tri
		//	Vertex vertex;
		//	//int vertexIndex[3] = { 0, 1, 2 };
		//	for (int j = 0; j < 3; j++)
		//	{
		//		int pointIndex = mesh->GetPolygonVertex(i, j);
		//		FbxVector4 controlPoint1 = controlPoints[pointIndex];

		//		FbxVector4 controlPoint = controlPoints[pointIndex];
		//		vertex.position.x = controlPoint[0];
		//		vertex.position.y = controlPoint[1];
		//		vertex.position.z = controlPoint[2];

		//		//FbxVector4 controlPoint2 = controlPoints[p2];
		//		//FbxVector4 controlPoint3 = controlPoints[p3];

		//		//FbxVector4 side1 = controlPoint2 - controlPoint1;
		//		//FbxVector4 side2 = controlPoint3 - controlPoint1;

		//		//XMVECTOR a = XMVectorSet(side1[0], side1[1], side1[2], side1[3]);
		//		//XMVECTOR b = XMVectorSet(side2[0], side2[1], side2[2], side2[3]);
		//		//XMFLOAT3 result;
		//		//XMStoreFloat3(&result, XMVector3Normalize(XMVector3Cross(a, b)));

		//		//vertex.normal = XMFLOAT3(normal[0], normal[1], normal[2]);
		//		vertices.push_back(vertex);

		//		//controlPoint = controlPoints[p2];
		//		//vertex.position.x = controlPoint[0];
		//		//vertex.position.y = controlPoint[1];
		//		//vertex.position.z = controlPoint[2];
		//		//vertex.normal = result;
		//		////vertex.normal = XMFLOAT3(normal[0], normal[1], normal[2]);
		//		////vertices.push_back(vertex);

		//		//controlPoint = controlPoints[p3];
		//		//vertex.position.x = controlPoint[0];
		//		//vertex.position.y = controlPoint[1];
		//		//vertex.position.z = controlPoint[2];
		//		//vertex.normal = result;
		//		//vertex.normal = XMFLOAT3(normal[0], normal[1], normal[2]);
		//		//vertices.push_back(vertex);

		//		indices.push_back(i * 3+j);
		//		//indices.push_back(i * 3 + 1);
		//		//indices.push_back(i * 3 + 2);
		//	}
		//}

		MeshData newMesh;
		newMesh.vertices = vertices;
		newMesh.indices = indices;

		return newMesh;
	}

}
#endif
