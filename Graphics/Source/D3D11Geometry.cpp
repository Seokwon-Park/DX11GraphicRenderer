#include "CommonHeaders.h"
#include "FbxLoader.h"

//LeftHanded CW
namespace graphics
{
	using namespace DirectX;

	MeshData Geometry::MakeSquare() {
		std::vector<XMFLOAT3> positions;
		std::vector<XMFLOAT3> normals;
		std::vector<XMFLOAT2> texcoords; // 텍스춰 좌표

		const float scale = 1.0f;

		// 앞면
		positions.push_back(XMFLOAT3(-1.0f, 1.0f, 0.0f));
		positions.push_back(XMFLOAT3(1.0f, 1.0f, 0.0f));
		positions.push_back(XMFLOAT3(1.0f, -1.0f, 0.0f));
		positions.push_back(XMFLOAT3(-1.0f, -1.0f, 0.0f));

		// Normals CW!! 
		normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));
		normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));
		normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));
		normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));

		// Texture Coordinates (Direct3D 9)
		// https://learn.microsoft.com/en-us/windows/win32/direct3d9/texture-coordinates
		texcoords.push_back(XMFLOAT2(0.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(1.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(1.0f, 1.0f));
		texcoords.push_back(XMFLOAT2(0.0f, 1.0f));

		MeshData meshData;

		for (size_t i = 0; i < positions.size(); i++) {
			Vertex v;
			v.position = positions[i];
			v.normal = normals[i];
			v.texcoord = texcoords[i];

			// v.color = colors[i];

			meshData.vertices.push_back(v);
		}
		meshData.indices = {
			0, 1, 2, 0, 2, 3, // 앞면
		};

		return meshData;
	}

	MeshData graphics::Geometry::MakeCube(float width, float height, float depth, float scale) {

		MeshData meshData;

		std::vector<XMFLOAT3> positions;
		std::vector<XMFLOAT3> normals;
		std::vector<XMFLOAT2> texcoords; // 텍스춰 좌표

		// 윗면
		positions.push_back(XMFLOAT3(-1.0f, 1.0f, -1.0f));
		positions.push_back(XMFLOAT3(-1.0f, 1.0f, 1.0f));
		positions.push_back(XMFLOAT3(1.0f, 1.0f, 1.0f));
		positions.push_back(XMFLOAT3(1.0f, 1.0f, -1.0f));
		normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
		normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
		normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
		normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(0.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(1.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(1.0f, 1.0f));
		texcoords.push_back(XMFLOAT2(0.0f, 1.0f));

		// 아랫면
		positions.push_back(XMFLOAT3(-1.0f, -1.0f, -1.0f));
		positions.push_back(XMFLOAT3(1.0f, -1.0f, -1.0f));
		positions.push_back(XMFLOAT3(1.0f, -1.0f, 1.0f));
		positions.push_back(XMFLOAT3(-1.0f, -1.0f, 1.0f));
		normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
		normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
		normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
		normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(0.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(1.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(1.0f, 1.0f));
		texcoords.push_back(XMFLOAT2(0.0f, 1.0f));

		// 앞면
		positions.push_back(XMFLOAT3(-1.0f, -1.0f, -1.0f));
		positions.push_back(XMFLOAT3(-1.0f, 1.0f, -1.0f));
		positions.push_back(XMFLOAT3(1.0f, 1.0f, -1.0f));
		positions.push_back(XMFLOAT3(1.0f, -1.0f, -1.0f));
		normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));
		normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));
		normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));
		normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));
		texcoords.push_back(XMFLOAT2(0.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(1.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(1.0f, 1.0f));
		texcoords.push_back(XMFLOAT2(0.0f, 1.0f));

		// 뒷면
		positions.push_back(XMFLOAT3(-1.0f, -1.0f, 1.0f));
		positions.push_back(XMFLOAT3(1.0f, -1.0f, 1.0f));
		positions.push_back(XMFLOAT3(1.0f, 1.0f, 1.0f));
		positions.push_back(XMFLOAT3(-1.0f, 1.0f, 1.0f));
		normals.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
		normals.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
		normals.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
		normals.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
		texcoords.push_back(XMFLOAT2(0.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(1.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(1.0f, 1.0f));
		texcoords.push_back(XMFLOAT2(0.0f, 1.0f));

		// 왼쪽
		positions.push_back(XMFLOAT3(-1.0f, -1.0f, 1.0f));
		positions.push_back(XMFLOAT3(-1.0f, 1.0f, 1.0f));
		positions.push_back(XMFLOAT3(-1.0f, 1.0f, -1.0f));
		positions.push_back(XMFLOAT3(-1.0f, -1.0f, -1.0f));
		normals.push_back(XMFLOAT3(-1.0f, 0.0f, 0.0f));
		normals.push_back(XMFLOAT3(-1.0f, 0.0f, 0.0f));
		normals.push_back(XMFLOAT3(-1.0f, 0.0f, 0.0f));
		normals.push_back(XMFLOAT3(-1.0f, 0.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(0.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(1.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(1.0f, 1.0f));
		texcoords.push_back(XMFLOAT2(0.0f, 1.0f));

		// 오른쪽
		positions.push_back(XMFLOAT3(1.0f, -1.0f, 1.0f));
		positions.push_back(XMFLOAT3(1.0f, -1.0f, -1.0f));
		positions.push_back(XMFLOAT3(1.0f, 1.0f, -1.0f));
		positions.push_back(XMFLOAT3(1.0f, 1.0f, 1.0f));
		normals.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
		normals.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
		normals.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
		normals.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(0.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(1.0f, 0.0f));
		texcoords.push_back(XMFLOAT2(1.0f, 1.0f));
		texcoords.push_back(XMFLOAT2(0.0f, 1.0f));



		for (size_t i = 0; i < positions.size(); i++) {
			Vertex v;
			v.position = positions[i];
			v.normal = normals[i];
			v.texcoord = texcoords[i];
			meshData.vertices.push_back(v);

		}

		meshData.indices = {
	0,  1,  2,  0,  2,  3,  // 윗면
	4,  5,  6,  4,  6,  7,  // 아랫면
	8,  9,  10, 8,  10, 11, // 앞면
	12, 13, 14, 12, 14, 15, // 뒷면
	16, 17, 18, 16, 18, 19, // 왼쪽
	20, 21, 22, 20, 22, 23  // 오른쪽
		};

		return meshData;
	}

	MeshData Geometry::MakeGrid(const float width, const float height,
		const int numSlices, const int numStacks) {

		// 안내: 실린더와 구 만들기 이해를 돕기 위해 후속 녹화한 강의입니다.

		// x-y 평면 (z = 0) 위에 격자 구조로 평면 만들기
		// 뒤에서 복잡한 지형으로 확장

		// 1단계: numStacks = 1 이고 numSlices만 고려해서 구현
		// 2단계: 2차원 바둑판 구조

		const float dx = width / numSlices;
		const float dy = height / numStacks;

		MeshData meshData;

		//Reference vertices & indices
		std::vector<Vertex>& vertices = meshData.vertices;
		std::vector<uint16_t>& indices = meshData.indices;

		// y = -0.5f * height 인 점들
		XMFLOAT3 stackStartPoint = XMFLOAT3(-0.5f * width, -0.5f * height, 0.0f);
		for (int j = 0; j <= numStacks; j++) {
			for (int i = 0; i <= numSlices; i++) {
				Vertex v;
				// x-y 평면에서 시작점을 x 방향으로 이동
				// v.position = stackStartPoint + Vector3(i * dx, 0.0f, 0.0f);
				XMStoreFloat3(&v.position, XMVector3Transform(XMLoadFloat3(&stackStartPoint), XMMatrixTranslation(dx * float(i), dy * float(j), 0.0f)));
				// v.position.z = sin(i * dx * 10.0f) * 0.1f;
				// 시점을 향하는 방향
				//v.normal = XMFLOAT3(cos(i * dx * 10.0f), 0.0f, -1.0f);
				// v.texcoord = Vector2(stackStartPoint.x + i * dx,
				// stackStartPoint.y);
				v.texcoord = XMFLOAT2(float(i) / numSlices, 1.0f - float(j) / numStacks);//uv좌표는 y축이 반대!!

				vertices.push_back(v);
			}
		}

		//// y = 0.5f * height 인 점들
		// stackStartPoint = Vector3(-0.5f * width, 0.5f * height, 0.0f);
		// for (int i = 0; i <= numSlices; i++) {
		//     Vertex v;

		//    // x-y 평면에서 시작점을 x 방향으로 이동
		//    v.position = Vector3::Transform(
		//        stackStartPoint,
		//        Matrix::CreateTranslation(Vector3(dx * float(i), 0.0f, 0.0f)));

		//    // 시점을 향하는 방향
		//    v.normal = Vector3(0.0f, 0.0f, -1.0f);
		//    // v.texcoord = Vector2(stackStartPoint.x + i * dx,
		//    stackStartPoint.y); v.texcoord = Vector2(float(i) / numSlices, 1.0f);

		//    vertices.push_back(v);
		//}

		// 인덱스 추가
		for (int j = 0; j < numStacks; j++) {
			for (int i = 0; i < numSlices; i++) {
				int w = numSlices + 1;
				// 첫번째 삼각형
				indices.push_back(i + w * j);
				indices.push_back(i + w * (j + 1));
				indices.push_back(i + w * (j + 1) + 1);

				// 두 번째 삼각형
				indices.push_back(i + w * j);
				indices.push_back(i + w * (j + 1) + 1);
				indices.push_back(i + w * j + 1);
			}
		}

		return meshData;
	}

	MeshData Geometry::MakeCylinder(const float bottomRadius, const float topRadius, float height, int numSlices, int numStacks)
	{
		MeshData meshData;

		//Reference vertices & indices
		std::vector<Vertex>& vertices = meshData.vertices;
		std::vector<uint16_t>& indices = meshData.indices;

		float theta_step = XM_2PI / numSlices;
		const float dy = height / numStacks;
		// y = -0.5f * height 인 점들
		XMFLOAT3 bottomStartPoint = XMFLOAT3(bottomRadius, -0.5f * height, 0.0f); // 하단
		XMFLOAT3 yStartPoint; // 상단
		for (int j = 0; j <= numStacks; j++) {
			Vertex v;
			XMStoreFloat3(&yStartPoint, XMVector3Transform(XMLoadFloat3(&bottomStartPoint), XMMatrixTranslation(0.0f, dy * j, 0.0f)));
			for (int i = 0; i <= numSlices; i++) {
				XMStoreFloat3(&v.position, XMVector3Transform(XMLoadFloat3(&yStartPoint), XMMatrixRotationY(theta_step * i)));

				XMStoreFloat3(&v.normal, XMVectorSet(v.position.x, -height * 0.5f + dy * j, v.position.z, 0.0f)
					- XMVectorSet(0.0f, -height * 0.5f + dy * j, 0.0f, 0.0f));

				v.texcoord = XMFLOAT2(float(i) / numSlices, 1.0f - float(j) / numStacks);
				vertices.push_back(v);
			}
			//XMStoreFloat3(&v.position, XMVector3Transform(XMLoadFloat3(&topStartPoint), XMMatrixTranslation(0.0f, dy * j, 0.0f)));
			//for (int i = 0; i <= numSlices; i++) {
			//    XMStoreFloat3(&v.position, XMVector3Transform(XMLoadFloat3(&topStartPoint), XMMatrixRotationY(theta_step * i)));

			//    XMStoreFloat3(&v.normal, XMVectorSet(v.position.x, height * 0.5f, v.position.z, 0.0f)
			//        - XMVectorSet(0.0f, height * 0.5f, 0.0f, 0.0f));
			//    // v.texcoord = Vector2(stackStartPoint.x + i * dx,
			//    // stackStartPoint.y);
			//    v.texcoord = XMFLOAT2(float(i) / numSlices, 1.0f);

			//    vertices.push_back(v);
			//}
		}

		for (int j = 0; j < numStacks; j++) {
			for (int i = 0; i < numSlices; i++) {
				// 첫번째 삼각형
				indices.push_back(i + numSlices * j);
				indices.push_back(i + numSlices * (j + 1));
				indices.push_back(i + 1 + numSlices * (j + 1));

				// 두 번째 삼각형
				indices.push_back(i + numSlices * j);
				indices.push_back(i + 1 + numSlices * (j + 1));
				indices.push_back(i + 1 + numSlices * j);
			}
		}

		return meshData;
	}

	MeshData Geometry::MakeSphere(const float radius, const int numSlices,
		const int numStacks)
	{
		MeshData meshData;

		//Reference vertices & indices
		std::vector<Vertex>& vertices = meshData.vertices;
		std::vector<uint16_t>& indices = meshData.indices;

		float thetaStack = -XM_PI / numStacks;
		float thetaSlice = -XM_2PI / numSlices;

		XMFLOAT3 startPoint = XMFLOAT3(0.0f, -radius, 0.0f); // 하단
		XMFLOAT3 stackPoint; // 하단
		Vertex v;

		for (int j = 0; j <= numStacks; j++) {

			XMStoreFloat3(&stackPoint, XMVector3Transform(XMLoadFloat3(&startPoint), XMMatrixRotationZ(thetaStack * j)));
			for (int i = 0; i <= numSlices; i++) {
				XMStoreFloat3(&v.position, XMVector3Transform(XMLoadFloat3(&stackPoint), XMMatrixRotationY(thetaSlice * i)));

				XMStoreFloat3(&v.normal, XMVector3Normalize(XMLoadFloat3(&v.position)));

				v.texcoord = XMFLOAT2(float(i) / numSlices, 1.0f - float(j) / numStacks);
				vertices.push_back(v);
			}

		}


		for (int j = 0; j <= numStacks; j++) {
			for (int i = 0; i < numSlices; i++) {
				// 첫번째 삼각형
				indices.push_back(i + numSlices * j);
				indices.push_back(i + numSlices * (j+1));
				indices.push_back(i + 1 + numSlices * (j + 1));

				// 두 번째 삼각형
				indices.push_back(i + numSlices * j);
				indices.push_back(i + 1 + numSlices * (j + 1));
				indices.push_back(i + 1 + numSlices * j);
			}
		}

		return meshData;
	}

	MeshData Geometry::SubdivideToSphere(const float radius,
		MeshData meshData) {

		using namespace DirectX;

		// 원점이 중심이라고 가정
		// 입력 받은 구 모델의 반지름 조절
		for (auto& v : meshData.vertices) {
			XMStoreFloat3(&v.position, XMVectorScale(XMLoadFloat3(&v.normal),radius));
		}

		// 구의 표면으로 옮기고 노멀 계산
		auto ProjectVertex = [&](Vertex& v) {
			v.normal = v.position;
			XMStoreFloat3(&v.normal, (XMVector3Normalize(XMLoadFloat3(&v.normal))));
			XMStoreFloat3(&v.position, XMVectorScale(XMLoadFloat3(&v.normal), radius));

			// 주의: 텍스춰가 이음매에서 깨집니다.
			// atan vs atan2
			// https://stackoverflow.com/questions/283406/what-is-the-difference-between-atan-and-atan2-in-c
			// const float theta = atan2f(v.position.z, v.position.x);
			// const float phi = acosf(v.position.y / radius);
			// v.texcoord.x = theta / XM_2PI;
			// v.texcoord.y = phi / XM_PI;
		};

		// 버텍스가 중복되는 구조로 구현
		MeshData newMesh;
		uint16_t count = 0;
		for (size_t i = 0; i < meshData.indices.size(); i += 3) {
			size_t i0 = meshData.indices[i];
			size_t i1 = meshData.indices[i + 1];
			size_t i2 = meshData.indices[i + 2];

			Vertex v0 = meshData.vertices[i0];
			Vertex v1 = meshData.vertices[i1];
			Vertex v2 = meshData.vertices[i2];

			Vertex v3;
			// 위치와 텍스춰 좌표 결정
			XMStoreFloat3(&v3.position, (XMLoadFloat3(&v0.position) + XMLoadFloat3(&v2.position)) * 0.5f);
			XMStoreFloat2(&v3.texcoord, (XMLoadFloat2(&v0.texcoord) + XMLoadFloat2(&v2.texcoord)) * 0.5f);

			Vertex v4;
			// 위치와 텍스춰 좌표 결정
			XMStoreFloat3(&v4.position, (XMLoadFloat3(&v0.position) + XMLoadFloat3(&v1.position)) * 0.5f);
			XMStoreFloat2(&v4.texcoord, (XMLoadFloat2(&v0.texcoord) + XMLoadFloat2(&v1.texcoord)) * 0.5f);


			Vertex v5;
			// 위치와 텍스춰 좌표 결정
			XMStoreFloat3(&v5.position, (XMLoadFloat3(&v1.position) + XMLoadFloat3(&v2.position)) * 0.5f);
			XMStoreFloat2(&v5.texcoord, (XMLoadFloat2(&v1.texcoord) + XMLoadFloat2(&v2.texcoord)) * 0.5f);

			ProjectVertex(v3);
			ProjectVertex(v4);
			ProjectVertex(v5);

			// 모든 버텍스 새로 추가
			newMesh.vertices.push_back(v0); 
			newMesh.vertices.push_back(v4); 
			newMesh.vertices.push_back(v3);

			newMesh.vertices.push_back(v4);
			newMesh.vertices.push_back(v1);
			newMesh.vertices.push_back(v5);

			newMesh.vertices.push_back(v3);
			newMesh.vertices.push_back(v5);
			newMesh.vertices.push_back(v2);

			newMesh.vertices.push_back(v3);
			newMesh.vertices.push_back(v4);
			newMesh.vertices.push_back(v5);

			// 인덱스 업데이트
			for (uint16_t j = 0; j < 12; j++) {
				newMesh.indices.push_back(j + count);
			}
			count += 12;
		}

		return newMesh;
	}

	//모델 로더
	std::vector<MeshData> Geometry::ReadModelFromFile(std::string filename)
	{
		FbxLoader fbx_context;

		fbx_context.Load(filename.c_str());

		return fbx_context.meshes;
	}


}