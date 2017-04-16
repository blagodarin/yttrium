#ifndef _include_yttrium_math_line3_h_
#define _include_yttrium_math_line3_h_

#include <yttrium/math/vector3.h>

namespace Yttrium
{
	class Line3
	{
	public:
		Vector3 a;
		Vector3 b;

		Line3() noexcept = default;
		constexpr Line3(const Vector3& va, const Vector3& vb) noexcept : a{va}, b{vb} {}

		bool plane_intersection(const Vector3& point, const Vector3& normal, Vector3& intersection) const noexcept
		{
			const auto direction = b - a;
			const auto d = dot_product(direction, normal);
			if (std::abs(d) < 1e-6)
				return false;
			const auto s = dot_product(point - a, normal) / d;
			if (s < 0 || s > 1)
				return false;
			intersection = a + s * direction;
			return true;
		}
	};
}

#endif
