#include "DXRay.h"

namespace graphics
{
	bool Ray::Intersects(const BoundingSphere& sphere, _Out_ float& Dist) const noexcept
	{
		XMVECTOR position = XMLoadFloat3(&m_pos);
		XMVECTOR direction = XMLoadFloat3(&m_dir);

		return sphere.Intersects(position, direction, Dist);
	}

}