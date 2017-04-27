#ifndef _include_yttrium_math_quad_h_
#define _include_yttrium_math_quad_h_

#include <yttrium/math/vector.h>

namespace Yttrium
{
	class Quad
	{
	public:
		Vector2 _a, _b, _c, _d;

		Quad() noexcept = default;
		constexpr Quad(const Vector2& a, const Vector2& b, const Vector2& c, const Vector2& d) noexcept : _a{a}, _b{b}, _c{c}, _d{d} {}
	};
}

#endif
