#ifndef _include_yttrium_math_point_h_
#define _include_yttrium_math_point_h_

namespace Yttrium
{
	class Point
	{
	public:
		int _x = 0;
		int _y = 0;

		constexpr Point() noexcept = default;
		constexpr Point(int x, int y) noexcept
			: _x{ x }, _y{ y } {}
	};

	constexpr bool operator==(const Point& a, const Point& b) noexcept { return a._x == b._x && a._y == b._y; }
	constexpr bool operator!=(const Point& a, const Point& b) noexcept { return !(a == b); }
}

#endif
