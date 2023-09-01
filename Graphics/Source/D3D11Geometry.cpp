#include "D3D11Geometry.h"
#include "D3D11Math.h"

//LeftHanded CW
namespace graphics
{
	using namespace DirectX;

	MeshData Geometry::MakePlane(float length, int axis1, int axis2) {
		std::vector<XMFLOAT3> positions;
		std::vector<XMFLOAT3> normals;
		std::vector<XMFLOAT2> texcoords; // 텍스춰 좌표

		const float scale = 1.0f;

		float xyz[3] = { 0.f, 0.f, 0.f };
		xyz[axis1] = 1.0f * length;
		xyz[axis2] = 1.0f * length;

		float vec[3] = { 0.f, 0.f, 0.f }; 
		vec[axis1] = 1.0f;
		XMVECTOR v1 = XMVectorSet( vec[0], vec[1], vec[2], 0.f);
		vec[axis1] = 0.f;
		vec[axis2] = 1.0f;
		XMVECTOR v2 = XMVectorSet( vec[0], vec[1], vec[2], 0.f);

		 
		XMFLOAT3 normal;
		XMStoreFloat3(&normal, XMVector3Cross(v2, v1));
		
		XMFLOAT2 test[4]
		{
			XMFLOAT2(-1.0f, 1.0f),
			XMFLOAT2(1.0f, 1.0f),
			XMFLOAT2(1.0f, -1.0f),
			XMFLOAT2(-1.0f, -1.0f)
		}; 

		for (int i = 0; i < 4; i++)
		{
			float xyz_copy[3] = { xyz[0], xyz[1], xyz[2] };
			// 앞면
			xyz_copy[axis1] *= test[i].x;
			xyz_copy[axis2] *= test[i].y;
			XMFLOAT3 temp = XMFLOAT3(xyz_copy[0], xyz_copy[1], xyz_copy[2]);
			positions.push_back(temp);

			//XMFLOAT3 res;
			//XMStoreFloat3(&res, XMVectorNegate(XMLoadFloat3(&normal)));
			// 법선
			normals.push_back(normal);
		}


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

		for (int i = 0; i < 2; i++)
		{
			int vertexIdx[3] = { 0, i+1, i+2 };
			for (int v = 0; v < 3; v++)
			{
				meshData.indices.push_back(vertexIdx[v]);
			}
		}

		return meshData;
	}

	MeshData graphics::Geometry::MakeCube(float width, float height, float depth) {

		MeshData meshData;

		std::vector<XMFLOAT3> positions;
		std::vector<XMFLOAT3> normals;
		std::vector<XMFLOAT2> texcoords;

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

		// x-y 평면 (z = 0) 위에 격자 구조로 평면 만들어서 확장

		const float dx = width / numSlices;
		const float dy = height / numStacks;

		MeshData meshData;

		//Reference vertices & indices
		std::vector<Vertex>& vertices = meshData.vertices;
		std::vector<uint32_t>& indices = meshData.indices;

		// y = -0.5f * height 인 점들
		XMFLOAT3 stackStartPoint = XMFLOAT3(-0.5f * width, -0.5f * height, 0.0f);
		for (int j = 0; j <= numStacks; j++) {
			for (int i = 0; i <= numSlices; i++) {
				Vertex v;
				// x-y 평면에서 시작점을 x 방향으로 이동
				// v.position = stackStartPoint + Vector3(i * dx, 0.0f, 0.0f);
				XMStoreFloat3(&v.position, XMVector3Transform(XMLoadFloat3(&stackStartPoint), XMMatrixTranslation(dx * float(i), dy * float(j), 0.0f)));
				
				// Calculate Sin Normal
				// https://www.khanacademy.org/math/multivariable-calculus/integrating-multivariable-functions/line-integrals-in-vector-fields-articles/a/constructing-a-unit-normal-vector-to-curve
				v.normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

				// 물결 모양 평면w
				// v.position.z = sin(i * dx * 10.0f) * 0.1f;
				// sin 그래프의 normal을 대입
				// v.normal = XMFLOAT3(cos(i * dx * 10.0f), 0.0f, -1.0f);
				v.texcoord = XMFLOAT2(float(i) / numSlices, 1.0f - float(j) / numStacks);//uv좌표는 y축이 반대!!

				vertices.push_back(v);
			}
		}

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
		std::vector<uint32_t>& indices = meshData.indices;

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

				//normal = modelPos - Center (Same height);
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
				int w = numSlices + 1;
				// 첫번째 삼각형
				indices.push_back(i + w * j);
				indices.push_back(i + w * (j + 1));
				indices.push_back(i + 1 + w * (j + 1));

				// 두 번째 삼각형
				indices.push_back(i + w * j);
				indices.push_back(i + 1 + w * (j + 1));
				indices.push_back(i + 1 + w * j);
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
		std::vector<uint32_t>& indices = meshData.indices;

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

	MeshData Geometry::MakeTetrahedron()
	{


		std::vector<XMFLOAT3> points = {
			XMFLOAT3({.5f, .5f, .5f}),
			XMFLOAT3({.5f, -.5f, -.5f}),
			XMFLOAT3({-.5f, .5f, -.5f}),
			XMFLOAT3({-.5f, -.5f, .5f}) 
		};

		XMVECTOR center = XMVectorSet(0.f,0.f,0.f,0.f);

		for (int i = 0; i < 4; i++) {
			center = XMVectorAdd(center, XMLoadFloat3(&points[i]));
		}
		XMVectorScale( center, 1.f/4.f);

		for (int i = 0; i < 4; i++) {
			XMStoreFloat3(&points[i], XMVectorSubtract(XMLoadFloat3(&points[i]), center));
		}

		MeshData meshData;

		for (int i = 0; i < points.size(); i++) {

			Vertex v;
			v.position = points[i];
			v.normal = v.position; // 중심이 원점
			XMStoreFloat3(&v.normal, XMVector3Normalize(XMLoadFloat3(&v.normal)));

			meshData.vertices.push_back(v);
		}

		meshData.indices = { 0, 1, 2, 3, 2, 1, 0, 3, 1, 0, 2, 3 };

		return meshData;
	}

	//https://math.stackexchange.com/questions/2174594/co-ordinates-of-the-vertices-an-icosahedron-relative-to-its-centroid
	MeshData Geometry::MakeIcosahedron()
	{
		float base = 1.f;
		float pi = (1.f + sqrt(5.f)) / 2 * base;

		std::vector<XMFLOAT3> points = {
			XMFLOAT3({pi,base,0}),
			XMFLOAT3({pi,-base,0}),
			XMFLOAT3({-pi,-base,0}),
			XMFLOAT3({-pi,base,0}),
			XMFLOAT3({base,0,pi}),
			XMFLOAT3({-base,0,pi}),
			XMFLOAT3({-base,0,-pi}),
			XMFLOAT3({base,0,-pi}),
			XMFLOAT3({0,pi,base}),
			XMFLOAT3({0,pi,-base}),
			XMFLOAT3({0,-pi,-base}),
			XMFLOAT3({0,-pi,base}),
		};

		MeshData meshData;

		for (size_t i = 0; i < points.size(); i++) {
			Vertex v;
			v.position = points[i];
			v.normal = v.position;
			XMStoreFloat3(&v.normal, XMVector3Normalize(XMLoadFloat3(&v.normal)));

			meshData.vertices.push_back(v);
		}

		//meshData.indices = { 1,  4,  0, 4,  9, 0, 4, 5,  9, 8, 5, 4,  1,  8, 4,
		//				   1,  10, 8, 10, 3, 8, 8, 3,  5, 3, 2, 5,  3,  7, 2,
		//				   3,  10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11, 6,  1, 0,
		//				   10, 1,  6, 11, 0, 9, 2, 11, 9, 5, 2, 9,  11, 2, 7 };
		meshData.indices = 
		{ 
			0, 9, 8,  0, 7, 9,  0, 1, 7,  0, 4, 1,  0, 8, 4,
			8, 9, 3,  9, 7, 6,  7, 1, 10,  1, 4, 11,  4, 8, 5,
			8, 3, 5,  9, 6, 3,  7, 10, 6,  1, 11, 10,  4, 5, 11,
			5, 3, 2,  3, 6, 2,  6, 10, 2,  10, 11, 2,  11, 5, 2  
		  };

		return meshData;
	}

	MeshData Geometry::SubdivideToSphere(const float radius,
		MeshData meshData) {

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

			// https://gamedev.stackexchange.com/questions/114412/how-to-get-uv-coordinates-for-sphere-cylindrical-projection
			const float theta = atan2f(v.position.z, v.position.x);
			const float phi = acosf(v.position.y / radius);
			v.texcoord.x = theta / XM_2PI;
			v.texcoord.y = phi / XM_PI;
		};

		// 버텍스가 중복되는 구조로 구현
		MeshData newMesh;
		uint32_t count = 0;
		for (size_t i = 0; i < meshData.indices.size(); i += 3) {
			size_t i0 = meshData.indices[i];
			size_t i1 = meshData.indices[i + 1];
			size_t i2 = meshData.indices[i + 2];

			Vertex v0 = meshData.vertices[i0];
			Vertex v1 = meshData.vertices[i1];
			Vertex v2 = meshData.vertices[i2];

			Vertex v3;
			// 위치와 텍스춰 좌표 결정
			XMStoreFloat3(&v3.position, (XMLoadFloat3(&v0.position) + XMLoadFloat3(&v2.position)) * .5f);
			XMStoreFloat2(&v3.texcoord, (XMLoadFloat2(&v0.texcoord) + XMLoadFloat2(&v2.texcoord)) * .5f);

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
			for (uint32_t j = 0; j < 12; j++) {
				newMesh.indices.push_back(j + count);
			}
			count += 12;
		}

		return newMesh;
	}

	//모델 로드
	std::vector<MeshData> Geometry::ReadModelFromFile(std::string filename)
	{
		std::vector<MeshData> meshes;
#if USE_FBX_SDK
		FbxLoader fbx_context;

		fbx_context.Load(filename.c_str());

		return fbx_context.meshes;
#elif USE_ASSIMP
		AssimpLoader modelLoader;

		modelLoader.Load(filename);

		meshes = modelLoader.meshes;
#endif
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
		return meshes;

	}



}