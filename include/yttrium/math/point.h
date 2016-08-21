/// \file
/// \brief 2D points.

#ifndef _include_yttrium_math_point_h_
#define _include_yttrium_math_point_h_

#include <utility>

namespace Yttrium
{
	/// Integral 2D point.
	class Point
	{
	public:
		Point() = default;
		Point(int x, int y) : _x(x), _y(y) {}

		int x() const { return _x; }
		int y() const { return _y; }

	private:
		int _x = 0;
		int _y = 0;
	};

	inline bool operator==(const Point& lhs, const Point& rhs)
	{
		return lhs.x() == rhs.x() && lhs.y() == rhs.y();
	}

	inline bool operator!=(const Point& lhs, const Point& rhs)
	{
		return !(lhs == rhs);
	}

	/// Floating-point 2D point.
	class PointF
	{
	public:
		PointF() = default;
		PointF(float x, float y) : _x(x), _y(y) {}
		explicit PointF(const Point& point) : PointF(point.x(), point.y()) {}

		float x() const { return _x; }
		float y() const { return _y; }

		float* data() { return &_x; }
		const float* data() const { return &_x; }

	private:
		float _x = 0;
		float _y = 0;
	};

	inline PointF operator/(const PointF& lhs, float rhs) { return { lhs.x() / rhs, lhs.y() / rhs }; }
	inline PointF operator/(const PointF& lhs, const std::pair<float, float>& rhs) { return { lhs.x() / rhs.first, lhs.y() / rhs.second }; }
}

#endif
