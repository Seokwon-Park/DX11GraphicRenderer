#pragma once
#include "D3D11CommonHeaders.h"

namespace graphics
{
	// Vertex���̴� ���
	struct BasicVertexConstantBuffer {
		XMMATRIX world;			//XMMATRIX float(4byte)*4*4 = 64byte �ƹ�ư 16���
		XMMATRIX invTranspose;
		XMMATRIX view;
		XMMATRIX projection;
	};

	static_assert((sizeof(BasicVertexConstantBuffer) % 16) == 0,
		"Constant Buffer size must be 16-byte aligned");

#define MAX_LIGHTS 3

	// ����
	struct Material {
		XMFLOAT3 ambient = XMFLOAT3(0.0f, 0.0f, 0.0f);  // 12byte
		float shininess = 256.0f;							// 4byte
		XMFLOAT3 diffuse = XMFLOAT3(0.0f, 0.0f, 0.0f);  // 12byte
		float pad1;										// 4byte
		XMFLOAT3 specular = XMFLOAT3(1.f, 1.f, 1.f);	// 12byte
		float pad2;
		XMFLOAT3 fresnelR0 = XMFLOAT3(1.0f, 0.71f, 0.29f); // Gold
		float pad3;// 4byte
		XMFLOAT3 edgeTint = XMFLOAT3(254.0f / 255.0f, 248.0f / 255.0f, 187.0f / 255.0f);
		float pad4;// 4byte
	};

	// ����
	struct Light {
		// ������ ũ�� ���� ���� (16 ����Ʈ �е�)
		XMFLOAT3 position = XMFLOAT3(0.f, 0.f, -2.f);	// 12byte
		float fallOffStart = 0.0f;						// 4byte
		XMFLOAT3 direction = XMFLOAT3(0.f, 0.f, 1.f);	// 12byte
		float fallOffEnd = 10.0f;						// 4byte
		XMFLOAT3 strength = XMFLOAT3(1.f, 1.f, 1.f);	// 12byte
		float spotPower = 100.0f;							// 4byte
	};

	// Pixel���̴� ���
	struct BasicPixelConstantBuffer {
		XMFLOAT3 eyeWorld;        // 12
		bool useTexture;		  // 4

		Material material;		  // 48 16*3
		Light lights[MAX_LIGHTS]; // 48 * MAX_LIGHTS

		XMFLOAT3 rimColor = XMFLOAT3(1.f, 1.f, 1.f); // 12
		float rimPower; // 4

		float rimStrength = 0.0f; // 4
		int useSmoothstep = 0; // 4
		int useRim = 0; // 4
		float dummy[1]; // 4
	};

	static_assert((sizeof(BasicPixelConstantBuffer) % 16) == 0,
		"Constant Buffer size must be 16-byte aligned");

	// Normal�׸��� ���� ���
	struct NormalVertexConstantBuffer {
		float scale = 0.1f;
		float pad[3];
	};

	static_assert((sizeof(NormalVertexConstantBuffer) % 16) == 0,
		"Constant Buffer size must be 16-byte aligned");

}