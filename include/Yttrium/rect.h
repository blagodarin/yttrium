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

	Rect(T left = 0, T top = 0, T width = 0, T height = 0) noexcept;

	///

	Rect(const Vector2<T> &size) noexcept;

	///

	Rect(const Vector2<T> &corner, const Vector2<T> &size) noexcept;

	///

	template <typename U>
	Rect(const Rect<U> &rect) noexcept;

public:

	///

	T bottom() const noexcept;

	///

	Vector2<T> bottom_left() const noexcept;

	///

	Vector2<T> bottom_right() const noexcept;

	///

	T height() const noexcept;

	///

	T left() const noexcept;

	///

	T outer_bottom() const noexcept;

	///

	Vector2<T> outer_bottom_left() const noexcept;

	///

	Vector2<T> outer_bottom_right() const noexcept;

	///

	T outer_right() const noexcept;

	///

	Vector2<T> outer_top_right() const noexcept;

	///

	T right() const noexcept;

	///

	T top() const noexcept;

	///

	Vector2<T> top_left() const noexcept;

	///

	Vector2<T> top_right() const noexcept;

	///

	T width() const noexcept;

public:

	///

	bool contains(const Vector2<T> &point) const noexcept;

	///

	bool contains(const Rect &rect) const noexcept;

public:

	///

	static Rect from_coords(const T &left, const T &top, const T &right, const T &bottom) noexcept;

	///

	static Rect from_outer_coords(const T &left, const T &top, const T &outer_right, const T &outer_bottom) noexcept;

private:

	T _left;
	T _top;
	T _outer_right;
	T _outer_bottom;
};

/// Rect of \c float.

typedef Rect<float> Rectf;

/// Rect of Dim.

typedef Rect<Dim> Area; // TODO: Find a better name.

////////////////////////////////////////////////////////////////////////////////

template <typename T>
Rect<T>::Rect(T left, T top, T width, T height) noexcept
	: _left(left)
	, _top(top)
	, _outer_right(left + width)
	, _outer_bottom(top + height)
{
}

template <typename T>
Rect<T>::Rect(const Vector2<T> &size) noexcept
	: _left(0)
	, _top(0)
	, _outer_right(size.width)
	, _outer_bottom(size.height)
{
}

template <typename T>
Rect<T>::Rect(const Vector2<T> &corner, const Vector2<T> &size) noexcept
	: _left(corner.x)
	, _top(corner.y)
	, _outer_right(corner.x + size.width)
	, _outer_bottom(corner.y + size.height)
{
}

template <typename T>
template <typename U>
Rect<T>::Rect(const Rect<U> &rect) noexcept
	: _left(rect._left)
	, _top(rect._top)
	, _outer_right(rect._outer_right)
	, _outer_bottom(rect._outer_bottom)
{
}

template <typename T>
T Rect<T>::bottom() const noexcept
{
	return _outer_bottom - 1;
}

template <typename T>
Vector2<T> Rect<T>::bottom_left() const noexcept
{
	return Vector2<T>(_left, _outer_bottom - 1);
}

template <typename T>
Vector2<T> Rect<T>::bottom_right() const noexcept
{
	return Vector2<T>(_outer_right - 1, _outer_bottom - 1);
}

template <typename T>
T Rect<T>::height() const noexcept
{
	return _outer_bottom - _top;
}

template <typename T>
T Rect<T>::left() const noexcept
{
	return _left;
}

template <typename T>
T Rect<T>::outer_bottom() const noexcept
{
	return _outer_bottom;
}

template <typename T>
Vector2<T> Rect<T>::outer_bottom_left() const noexcept
{
	return Vector2<T>(_left, _outer_bottom);
}

template <typename T>
Vector2<T> Rect<T>::outer_bottom_right() const noexcept
{
	return Vector2<T>(_outer_right, _outer_bottom);
}

template <typename T>
T Rect<T>::outer_right() const noexcept
{
	return _outer_right;
}

template <typename T>
Vector2<T> Rect<T>::outer_top_right() const noexcept
{
	return Vector2<T>(_outer_right, _top);
}

template <typename T>
T Rect<T>::right() const noexcept
{
	return _outer_right - 1;
}

template <typename T>
T Rect<T>::top() const noexcept
{
	return _top;
}

template <typename T>
Vector2<T> Rect<T>::top_left() const noexcept
{
	return Vector2<T>(_left, _top);
}

template <typename T>
Vector2<T> Rect<T>::top_right() const noexcept
{
	return Vector2<T>(_outer_right - 1, _top);
}

template <typename T>
bool Rect<T>::contains(const Vector2<T> &point) const noexcept
{
	return _left <= point.x && point.x < _outer_right
		&& _top <= point.y && point.y < _outer_bottom;
}

template <typename T>
bool Rect<T>::contains(const Rect &rect) const noexcept
{
	return _left <= rect._left && rect._outer_right <= _outer_right
		&& _top <= rect._top && rect._outer_bottom <= _outer_bottom;
}

template <typename T>
T Rect<T>::width() const noexcept
{
	return _outer_right - _left;
}

template <typename T>
Rect<T> Rect<T>::from_coords(const T &left, const T &top, const T &right, const T &bottom) noexcept
{
	return Rect(left, top, right - left + 1, bottom - top + 1);
}

template <typename T>
Rect<T> Rect<T>::from_outer_coords(const T &left, const T &top, const T &outer_right, const T &outer_bottom) noexcept
{
	return Rect(left, top, outer_right - left, outer_bottom - top);
}

} // namespace Yttrium

#endif // __Y_RECT_H
