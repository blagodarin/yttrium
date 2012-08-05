/// \file
/// \brief Rectangles.

#ifndef __Y_RECT_H
#define __Y_RECT_H

#include <Yttrium/vector.h>

namespace Yttrium
{

/// Rectangle.

template <typename T>
class Rect
{
public:

	///

	Rect(T x = 0, T y = 0, T width = 0, T height = 0) noexcept
		: _x1(x)
		, _y1(y)
		, _x2(x + width)
		, _y2(y + height)
	{
	}

	///

	Rect(const Vector2<T> &size) noexcept
		: _x1(0)
		, _y1(0)
		, _x2(size.width)
		, _y2(size.height)
	{
	}

	///

	Rect(const Vector2<T> &corner, const Vector2<T> &size) noexcept
		: _x1(corner.x)
		, _y1(corner.y)
		, _x2(corner.x + size.width)
		, _y2(corner.y + size.height)
	{
	}

	///

	template <typename U>
	Rect(const Rect<U> &rect) noexcept
		: _x1(rect._x1)
		, _y1(rect._y1)
		, _x2(rect._x2)
		, _y2(rect._y2)
	{
	}

	///

	T height() const noexcept
	{
		return _y2 - _y1;
	}

	///

	T width() const noexcept
	{
		return _x2 - _x1;
	}

	///

	T x() const noexcept
	{
		return _x1;
	}

	///

	T x1() const noexcept
	{
		return _x1;
	}

	///

	T x2() const noexcept
	{
		return _x2;
	}

	///

	T y() const noexcept
	{
		return _y1;
	}

	///

	T y1() const noexcept
	{
		return _y1;
	}

	///

	T y2() const noexcept
	{
		return _y2;
	}

public:

	///

	bool contains(const Vector2<T> &point) const noexcept
	{
		return _x1 <= point.x && point.x <= _x2
			&& _y1 <= point.y && point.y <= _y2;
	}

	///

	bool contains(const Rect &rect) const noexcept
	{
		return _x1 <= rect._x1 && rect._x2 <= _x2
			&& _y1 <= rect._y1 && rect._y2 <= _y2;
	}

public:

	///

	static Rect from_coords(const T &x1, const T &y1, const T &x2, const T &y2) noexcept
	{
		return Rect(x1, y1, x2 - x1, y2 - y1);
	}

private:

	T _x1;
	T _y1;
	T _x2;
	T _y2;
};

/// Rect of \c float.

typedef Rect<float> Rectf;

/// Rect of Dim.

typedef Rect<Dim> Area; // TODO: Find a better name.

} // namespace Yttrium

#endif // __Y_RECT_H
