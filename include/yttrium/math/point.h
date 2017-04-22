#ifndef _include_yttrium_math_point_h_
#define _include_yttrium_math_point_h_

#include <yttrium/math/vector.h>

namespace Yttrium
{
	class Point
	{
	public:
		int _x = 0;
		int _y = 0;

		constexpr Point() noexcept = default;
		constexpr Point(int x, int y) noexcept : _x{x}, _y{y} {}
	};

	class PointF
	{
	public:
		float _x = 0;
		float _y = 0;

		constexpr PointF() noexcept = default;
		constexpr PointF(float x, float y) noexcept : _x{x}, _y{y} {}
		constexpr explicit PointF(const Point& p) noexcept : PointF{static_cast<float>(p._x), static_cast<float>(p._y)} {}
	};

	constexpr bool operator==(const Point& a, const Point& b) noexcept { return a._x == b._x && a._y == b._y; }
	constexpr bool operator!=(const Point& a, const Point& b) noexcept { return !(a == b); }

	constexpr PointF operator/(const PointF& p, float s) noexcept { return {p._x / s, p._y / s}; }
	constexpr PointF operator/(const PointF& p, const Vector2& v) noexcept { return {p._x / v.x, p._y / v.y}; }
}

#endif
