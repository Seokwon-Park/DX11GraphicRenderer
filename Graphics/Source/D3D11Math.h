#pragma once
#include "CommonHeaders.h"

namespace graphics
{
	XMFLOAT3 operator+=(XMFLOAT3& a, XMFLOAT3& b);
	XMFLOAT3 operator-=(XMFLOAT3& a, XMFLOAT3& b);
	XMFLOAT3 operator*=(XMFLOAT3& a, float b);
	XMFLOAT3 operator/=(XMFLOAT3& a, float b);
}