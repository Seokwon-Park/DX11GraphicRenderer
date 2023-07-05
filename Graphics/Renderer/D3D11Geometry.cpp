#include "CommonHeaders.h"


namespace graphics
{
	using namespace DirectX;

	MeshData Geometry::MakeSquare() {
		std::vector<XMFLOAT3> positions;
		std::vector<XMFLOAT3> colors;
		std::vector<XMFLOAT3> normals;
		std::vector<XMFLOAT2> texcoords; // �ؽ��� ��ǥ

		const float scale = 1.0f;

		// �ո�
		positions.push_back(XMFLOAT3(-1.0f, 1.0f, 0.0f));
		positions.push_back(XMFLOAT3(1.0f, 1.0f, 0.0f));
		positions.push_back(XMFLOAT3(1.0f, -1.0f, 0.0f));
		positions.push_back(XMFLOAT3(-1.0f, -1.0f, 0.0f));
		colors.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
		colors.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
		colors.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
		colors.push_back(XMFLOAT3(0.0f, 0.0f, 1.0f));
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
			0, 1, 2, 0, 2, 3, // �ո�
		};

		return meshData;
	}

	MeshData graphics::Geometry::MakeCube(float width, float height, float depth, float scale) {

		MeshData meshData;

        std::vector<XMFLOAT3> positions;
        std::vector<XMFLOAT3> colors;
        std::vector<XMFLOAT3> normals;
        std::vector<XMFLOAT2> texcoords; // �ؽ��� ��ǥ

        // ����
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

        // �Ʒ���
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

        // �ո�
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

        // �޸�
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

        // ����
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

        // ������
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
    0,  1,  2,  0,  2,  3,  // ����
    4,  5,  6,  4,  6,  7,  // �Ʒ���
    8,  9,  10, 8,  10, 11, // �ո�
    12, 13, 14, 12, 14, 15, // �޸�
    16, 17, 18, 16, 18, 19, // ����
    20, 21, 22, 20, 22, 23  // ������
        };

		return meshData;
	}
}