#ifndef _include_yttrium_math_plane_h_
#define _include_yttrium_math_plane_h_

#include <yttrium/math/vector.h>

namespace Yttrium
{
	class Plane
	{
	public:
		Vector3 _normal;
		float _offset;

		Plane() noexcept = default;
		Plane(const Vector3& normal, const Vector3& origin) noexcept
			: _normal{ normalize(normal) }, _offset{ dot_product(_normal, origin) } {}

		constexpr float distance_to(const Vector3& point) const noexcept { return dot_product(_normal, point) - _offset; }
	};
}

#endif
