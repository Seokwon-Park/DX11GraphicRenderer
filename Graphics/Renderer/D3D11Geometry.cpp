#include "CommonHeaders.h"


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
        std::vector<XMFLOAT3> colors;
        std::vector<XMFLOAT3> normals;
        std::vector<XMFLOAT2> texcoords; // 텍스춰 좌표

        // 윗면
        positions.push_back(XMFLOAT3(-1.0f, 1.0f, -1.0f) );
        positions.push_back(XMFLOAT3(-1.0f, 1.0f, 1.0f) );
        positions.push_back(XMFLOAT3(1.0f, 1.0f, 1.0f) );
        positions.push_back(XMFLOAT3(1.0f, 1.0f, -1.0f) );
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
        texcoords.push_back(XMFLOAT2(0.0f, 0.0f));
        texcoords.push_back(XMFLOAT2(1.0f, 0.0f));
        texcoords.push_back(XMFLOAT2(1.0f, 1.0f));
        texcoords.push_back(XMFLOAT2(0.0f, 1.0f));

        // 아랫면
        positions.push_back(XMFLOAT3(-1.0f, -1.0f, -1.0f) );
        positions.push_back(XMFLOAT3(1.0f, -1.0f, -1.0f) );
        positions.push_back(XMFLOAT3(1.0f, -1.0f, 1.0f) );
        positions.push_back(XMFLOAT3(-1.0f, -1.0f, 1.0f) );
        colors.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
        colors.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
        colors.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
        colors.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
        normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
        texcoords.push_back(XMFLOAT2(0.0f, 0.0f));
        texcoords.push_back(XMFLOAT2(1.0f, 0.0f));
        texcoords.push_back(XMFLOAT2(1.0f, 1.0f));
        texcoords.push_back(XMFLOAT2(0.0f, 1.0f));

        // 앞면
        positions.push_back(XMFLOAT3(-1.0f, -1.0f, -1.0f) );
        positions.push_back(XMFLOAT3(-1.0f, 1.0f, -1.0f) );
        positions.push_back(XMFLOAT3(1.0f, 1.0f, -1.0f) );
        positions.push_back(XMFLOAT3(1.0f, -1.0f, -1.0f) );
        colors.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
        colors.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
        colors.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
        colors.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
        normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));
        normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));
        normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));
        normals.push_back(XMFLOAT3(0.0f, 0.0f, -1.0f));
        texcoords.push_back(XMFLOAT2(0.0f, 0.0f));
        texcoords.push_back(XMFLOAT2(1.0f, 0.0f));
        texcoords.push_back(XMFLOAT2(1.0f, 1.0f));
        texcoords.push_back(XMFLOAT2(0.0f, 1.0f));

        // 뒷면
        positions.push_back(XMFLOAT3(-1.0f, -1.0f, 1.0f) );
        positions.push_back(XMFLOAT3(1.0f, -1.0f, 1.0f) );
        positions.push_back(XMFLOAT3(1.0f, 1.0f, 1.0f) );
        positions.push_back(XMFLOAT3(-1.0f, 1.0f, 1.0f) );
        colors.push_back(XMFLOAT3(0.0f, 1.0f, 1.0f));
        colors.push_back(XMFLOAT3(0.0f, 1.0f, 1.0f));
        colors.push_back(XMFLOAT3(0.0f, 1.0f, 1.0f));
        colors.push_back(XMFLOAT3(0.0f, 1.0f, 1.0f));
        normals.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
        normals.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
        normals.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
        normals.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
        texcoords.push_back(XMFLOAT2(0.0f, 0.0f));
        texcoords.push_back(XMFLOAT2(1.0f, 0.0f));
        texcoords.push_back(XMFLOAT2(1.0f, 1.0f));
        texcoords.push_back(XMFLOAT2(0.0f, 1.0f));

        // 왼쪽
        positions.push_back(XMFLOAT3(-1.0f, -1.0f, 1.0f) );
        positions.push_back(XMFLOAT3(-1.0f, 1.0f, 1.0f) );
        positions.push_back(XMFLOAT3(-1.0f, 1.0f, -1.0f) );
        positions.push_back(XMFLOAT3(-1.0f, -1.0f, -1.0f) );
        colors.push_back(XMFLOAT3(1.0f, 1.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 1.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 1.0f, 0.0f));
        colors.push_back(XMFLOAT3(1.0f, 1.0f, 0.0f));
        normals.push_back(XMFLOAT3(-1.0f, 0.0f, 0.0f));
        normals.push_back(XMFLOAT3(-1.0f, 0.0f, 0.0f));
        normals.push_back(XMFLOAT3(-1.0f, 0.0f, 0.0f));
        normals.push_back(XMFLOAT3(-1.0f, 0.0f, 0.0f));
        texcoords.push_back(XMFLOAT2(0.0f, 0.0f));
        texcoords.push_back(XMFLOAT2(1.0f, 0.0f));
        texcoords.push_back(XMFLOAT2(1.0f, 1.0f));
        texcoords.push_back(XMFLOAT2(0.0f, 1.0f));

        // 오른쪽
        positions.push_back(XMFLOAT3(1.0f, -1.0f, 1.0f) );
        positions.push_back(XMFLOAT3(1.0f, -1.0f, -1.0f) );
        positions.push_back(XMFLOAT3(1.0f, 1.0f, -1.0f) );
        positions.push_back(XMFLOAT3(1.0f, 1.0f, 1.0f) );
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 1.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 1.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 1.0f));
        colors.push_back(XMFLOAT3(1.0f, 0.0f, 1.0f));
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
                XMStoreFloat3(&v.position, XMVector3Transform(XMLoadFloat3(&stackStartPoint), XMMatrixTranslation( dx * float(i), dy * float(j), 0.0f)));
                v.position.z = sin(i * dx * 10.0f) * 0.1f;
                // 시점을 향하는 방향
                v.normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
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

}