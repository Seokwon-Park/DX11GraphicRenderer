#include "CommonHeaders.h"


namespace graphics
{
	using namespace DirectX;

	MeshData Geometry::MakeSquare() {
		std::vector<XMFLOAT3> positions;
		std::vector<XMFLOAT3> normals;
		std::vector<XMFLOAT2> texcoords; // �ؽ��� ��ǥ

		const float scale = 1.0f;

		// �ո�
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

    MeshData Geometry::MakeGrid(const float width, const float height,
        const int numSlices, const int numStacks) {

        // �ȳ�: �Ǹ����� �� ����� ���ظ� ���� ���� �ļ� ��ȭ�� �����Դϴ�.

        // x-y ��� (z = 0) ���� ���� ������ ��� �����
        // �ڿ��� ������ �������� Ȯ��

        // 1�ܰ�: numStacks = 1 �̰� numSlices�� ����ؼ� ����
        // 2�ܰ�: 2���� �ٵ��� ����

        const float dx = width / numSlices;
        const float dy = height / numStacks;

        MeshData meshData;

        //Reference vertices & indices
        std::vector<Vertex>& vertices = meshData.vertices;
        std::vector<uint16_t>& indices = meshData.indices;

        // y = -0.5f * height �� ����
        XMFLOAT3 stackStartPoint = XMFLOAT3(-0.5f * width, -0.5f * height, 0.0f);
        for (int j = 0; j <= numStacks; j++) {
            for (int i = 0; i <= numSlices; i++) {
                Vertex v;
                // x-y ��鿡�� �������� x �������� �̵�
                // v.position = stackStartPoint + Vector3(i * dx, 0.0f, 0.0f);
                XMStoreFloat3(&v.position, XMVector3Transform(XMLoadFloat3(&stackStartPoint), XMMatrixTranslation( dx * float(i), dy * float(j), 0.0f)));
                v.position.z = sin(i * dx * 10.0f) * 0.1f;
                // ������ ���ϴ� ����
                v.normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
                // v.texcoord = Vector2(stackStartPoint.x + i * dx,
                // stackStartPoint.y);
                v.texcoord = XMFLOAT2(float(i) / numSlices, 1.0f - float(j) / numStacks);//uv��ǥ�� y���� �ݴ�!!

                vertices.push_back(v);
            }
        }

        //// y = 0.5f * height �� ����
        // stackStartPoint = Vector3(-0.5f * width, 0.5f * height, 0.0f);
        // for (int i = 0; i <= numSlices; i++) {
        //     Vertex v;

        //    // x-y ��鿡�� �������� x �������� �̵�
        //    v.position = Vector3::Transform(
        //        stackStartPoint,
        //        Matrix::CreateTranslation(Vector3(dx * float(i), 0.0f, 0.0f)));

        //    // ������ ���ϴ� ����
        //    v.normal = Vector3(0.0f, 0.0f, -1.0f);
        //    // v.texcoord = Vector2(stackStartPoint.x + i * dx,
        //    stackStartPoint.y); v.texcoord = Vector2(float(i) / numSlices, 1.0f);

        //    vertices.push_back(v);
        //}

        // �ε��� �߰�
        for (int j = 0; j < numStacks; j++) {
            for (int i = 0; i < numSlices; i++) {
                int w = numSlices + 1;
                // ù��° �ﰢ��
                indices.push_back(i + w * j);
                indices.push_back(i + w * (j + 1));
                indices.push_back(i + w * (j + 1) + 1);

                // �� ��° �ﰢ��
                indices.push_back(i + w * j);
                indices.push_back(i + w * (j + 1) + 1);
                indices.push_back(i + w * j + 1);
            }
        }

        return meshData;
    }

}