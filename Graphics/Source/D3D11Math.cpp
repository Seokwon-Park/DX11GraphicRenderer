#include "D3D11Math.h"

namespace graphics
{
	XMFLOAT3 operator+=(XMFLOAT3& a, XMFLOAT3& b)
	{
		const XMVECTOR v1 = XMLoadFloat3(&a);
		const XMVECTOR v2 = XMLoadFloat3(&b);
		const XMVECTOR X = XMVectorAdd(v1, v2);
		XMFLOAT3 res;
		XMStoreFloat3(&res, X);

		return res;
	}

	XMFLOAT3 operator-=(XMFLOAT3& a, XMFLOAT3& b)
	{
		const XMVECTOR v1 = XMLoadFloat3(&a);
		const XMVECTOR v2 = XMLoadFloat3(&b);
		const XMVECTOR X = XMVectorSubtract(v1, v2);
		XMFLOAT3 res;
		XMStoreFloat3(&res, X);

		return res;
	}

	XMFLOAT3 operator*=(XMFLOAT3& a, float b)
	{
		const XMVECTOR v1 = XMLoadFloat3(&a);
		const XMVECTOR X = XMVectorScale(v1, b);
		XMFLOAT3 res;
		XMStoreFloat3(&res, X);

		return res;
	}

	XMFLOAT3 operator/=(XMFLOAT3& a, float b)
	{
		const XMVECTOR v1 = XMLoadFloat3(&a);
		const XMVECTOR X = XMVectorScale(v1, 1.f / b);
		XMFLOAT3 res;
		XMStoreFloat3(&res, X);

		return res;
	}
}