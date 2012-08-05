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

	T x;
	T y;
	T width;
	T height;

public:

	Rect(T x = 0, T y = 0, T width = 0, T height = 0)
		: x(x)
		, y(y)
		, width(width)
		, height(height)
	{
	}

	Rect(const Vector2<T> &size)
		: x(0)
		, y(0)
		, width(size.width)
		, height(size.height)
	{
	}

	Rect(const Vector2<T> &corner, const Vector2<T> &size)
		: x(corner.x)
		, y(corner.y)
		, width(size.width)
		, height(size.height)
	{
	}

	template <typename U>
	Rect(const Rect<U> &r)
		: x(r.x)
		, y(r.y)
		, width(r.width)
		, height(r.height)
	{
	}

public:

	bool contains(const Vector2<T> &v) const
	{
		return (x <= v.x && y <= v.y && x + width >= v.x && y + height >= v.y);
	}

	bool contains(const Rect &r) const
	{
		return (x <= r.x && y <= r.y && x + width >= r.x + r.width && y + height >= r.x + r.height);
	}

public:

	static Rect from_coords(const T &x1, const T &y1, const T &x2, const T &y2)
	{
		return Rect(x1, y1, x2 - x1, y2 - y1);
	}
};

/// Rect of \c float.

typedef Rect<float> Rectf;

/// Rect of Dim.

typedef Rect<Dim> Area; // TODO: Find a better name.

} // namespace Yttrium

#endif // __Y_RECT_H
