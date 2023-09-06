#pragma once
#include "D3D11CommonHeaders.h"


namespace graphics
{
	using namespace DirectX;
	class Ray
	{
	public:
		Ray() noexcept : m_pos(0, 0, 0), m_dir(0, 0, 1) {}
		Ray(const XMFLOAT3& pos, const XMFLOAT3& dir) noexcept : m_pos(pos), m_dir(dir) {}

		Ray(const Ray&) = default;
		Ray& operator=(const Ray&) = default;

		Ray(Ray&&) = default;
		Ray& operator=(Ray&&) = default;

		bool Intersects(const BoundingSphere& sphere, _Out_ float& Dist) const noexcept;

		XMFLOAT3 m_pos;
		XMFLOAT3 m_dir;
	};
}