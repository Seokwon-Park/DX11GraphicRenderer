#include "CommonHeaders.h"
#include "Geometry.h"

namespace graphics
{
	using namespace DirectX;



	MeshData graphics::Geometry::MakeCube(float width, float height, float depth, float scale) {

		MeshData meshData;

		// position, normal, uvcoord
		XMFLOAT3 pos[8]
		{
			{-width * 0.5f, height * 0.5f, -depth * 0.5f},
			{-width * 0.5f, height * 0.5f, depth * 0.5f},
			{width * 0.5f, height * 0.5f, depth * 0.5f},
			{width * 0.5f, height * 0.5f, -depth * 0.5f},
			{-width * 0.5f, -height * 0.5f, -depth * 0.5f},
			{-width * 0.5f, -height * 0.5f, depth * 0.5f},
			{width * 0.5f, -height * 0.5f, depth * 0.5f},
			{width * 0.5f, -height * 0.5f, -depth * 0.5f},
		};

		XMFLOAT3 normal[6]
		{
			{ 0.f, 1.f, 0.f }, //up 
			{ 0.f, -1.f, 0.f }, //down 
			{ 0.f, 0.f, 1.f }, //front 
			{ 0.f, 0.f, -1.f }, //back 
			{ -1.f, 0.f, 0.f }, //left 
			{ 1.f, 0.f, 0.f }, //right 
		};

		XMFLOAT2 uv[4]
		{
			{0.0f, 1.0f}, // left down
			{0.0f, 0.0f}, // left up
			{1.0f, 0.0f}, // right up
			{1.0f, 1.0f}, // right down
		};

		std::vector<XMFLOAT3> positions
		{
			pos[0], pos[1], pos[2], pos[0], pos[2], pos[3], // up
			pos[5], pos[4], pos[7], pos[5], pos[7], pos[6], // down
			pos[4], pos[0], pos[3], pos[4], pos[3], pos[7], // front
			pos[1], pos[5], pos[6], pos[1], pos[6], pos[2], // back
			pos[4], pos[5], pos[1], pos[4], pos[1], pos[0], // left
			pos[3], pos[2], pos[6], pos[3], pos[6], pos[7], // right
		};

		std::vector<XMFLOAT3> normals
		{
			normal[0],normal[0],normal[0],normal[0],normal[0],normal[0],
			normal[1],normal[1],normal[1],normal[1],normal[1],normal[1],
			normal[2],normal[2],normal[2],normal[2],normal[2],normal[2],
			normal[3],normal[3],normal[3],normal[3],normal[3],normal[3],
			normal[4],normal[4],normal[4],normal[4],normal[4],normal[4],
			normal[5],normal[5],normal[5],normal[5],normal[5],normal[5],
		};


		std::vector<XMFLOAT2> texcoords
		{
			uv[0], uv[1], uv[2], uv[0], uv[2], uv[3],
			uv[0], uv[1], uv[2], uv[0], uv[2], uv[3],
			uv[0], uv[1], uv[2], uv[0], uv[2], uv[3],
			uv[0], uv[1], uv[2], uv[0], uv[2], uv[3],
			uv[0], uv[1], uv[2], uv[0], uv[2], uv[3],
			uv[0], uv[1], uv[2], uv[0], uv[2], uv[3],
		};



		std::vector<uint16_t> indices = {
			0, 1, 2, 0, 2, 3, // up
			5, 4, 7, 5, 7, 6, // down
			4, 0, 3, 4, 3, 7, // front
			1, 5, 6, 1, 6, 2, // back
			4, 5, 1, 4, 1, 0, // left
			3, 2, 6, 3, 6, 7, // right
		};

		for (size_t i = 0; i < positions.size(); i++) {
			Vertex v;
			v.position = positions[i];
			v.normal = normals[i];
			v.texcoord = texcoords[i];
			meshData.vertices.push_back(v);
		}

		return meshData;
	}
}