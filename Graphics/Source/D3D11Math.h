#pragma once
#include "D3D11CommonHeaders.h"

namespace graphics
{
	using namespace DirectX;

	XMFLOAT3 operator+=(XMFLOAT3& a, XMFLOAT3& b);
	XMFLOAT3 operator-=(XMFLOAT3& a, XMFLOAT3& b);
	XMFLOAT3 operator*=(XMFLOAT3& a, float b);
	XMFLOAT3 operator/=(XMFLOAT3& a, float b);

	XMFLOAT3 CreateXMFLOAT3(float x);

}