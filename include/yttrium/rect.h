/// \file
/// \brief Rectangles.

#ifndef __Y_RECT_H
#define __Y_RECT_H

#include <yttrium/vector.h>

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

	Vector2<T> center() const noexcept;

	///

	T height() const noexcept;

	///

	bool is_empty() const noexcept { return _left >= _right || _top >= _bottom; }

	///

	T left() const noexcept;

	///

	T right() const noexcept;

	///

	void set_coords(T left, T top, T right, T bottom) noexcept;

	///

	void set_height(T height) noexcept;

	///

	void set_left(T left) noexcept;

	///

	void set_size(T width, T height) noexcept;

	///

	void set_size(const Vector2<T> &size) noexcept;

	///

	void set_top(T top) noexcept;

	///

	void set_top_left(T left, T top) noexcept;

	///

	void set_top_left(const Vector2<T> &top_left) noexcept;

	///

	void set_width(T width) noexcept;

	///

	Vector2<T> size() const noexcept;

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

	///

	bool fast_contains(const Vector2<T> &point) const noexcept;

	/// Find whether the rects intersect.

	bool fast_intersects(const Rect &rect) const noexcept;

	/// Find whether the rects intersect.
	/// \param rect
	/// \return \c true if this rect intersects \a rect.
	/// \note This only works for integer rects with <tt>left <= right</tt> and <tt>top <= bottom</tt>.
	/// It also assumes the subtraction result of any two coordinates along the same axis is valid.
	/// \note This function reports an intersection of a null rect with itself
	/// or with a non-null rect if the point of the null rect lies inside the non-null rect.

	bool fastest_intersects(const Rect &rect) const noexcept;

	/// Find whether the rects intersect.
	/// \param rect
	/// \return \c true if this rect intersects \a rect.
	/// \note This function reports no intersection of a null rect with itself,
	/// though it does report an intersection for a null rect with a non-null rect
	/// if the point of the null rect lies inside the non-null rect.

	bool intersects(const Rect &rect) const noexcept;

public:

	///

	static Rect from_coords(T left, T top, T right, T bottom) noexcept;

private:

	T _left;
	T _top;
	T _right;
	T _bottom;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
Rect<T>::Rect(T left, T top, T width, T height) noexcept
	: _left(left)
	, _top(top)
	, _right(left + width)
	, _bottom(top + height)
{
}

template <typename T>
Rect<T>::Rect(const Vector2<T> &size) noexcept
	: _left(0)
	, _top(0)
	, _right(size.x)
	, _bottom(size.y)
{
}

template <typename T>
Rect<T>::Rect(const Vector2<T> &corner, const Vector2<T> &size) noexcept
	: _left(corner.x)
	, _top(corner.y)
	, _right(corner.x + size.x)
	, _bottom(corner.y + size.y)
{
}

template <typename T>
template <typename U>
Rect<T>::Rect(const Rect<U> &rect) noexcept
	: _left(rect.left())
	, _top(rect.top())
	, _right(rect.right())
	, _bottom(rect.bottom())
{
}

template <typename T>
T Rect<T>::bottom() const noexcept
{
	return _bottom;
}

template <typename T>
Vector2<T> Rect<T>::bottom_left() const noexcept
{
	return Vector2<T>(_left, _bottom);
}

template <typename T>
Vector2<T> Rect<T>::bottom_right() const noexcept
{
	return Vector2<T>(_right, _bottom);
}

template <typename T>
Vector2<T> Rect<T>::center() const noexcept
{
	return Vector2<T>((_left + _right) / 2, (_top + _bottom) / 2);
}

template <typename T>
T Rect<T>::height() const noexcept
{
	return _bottom - _top;
}

template <typename T>
T Rect<T>::left() const noexcept
{
	return _left;
}

template <typename T>
T Rect<T>::right() const noexcept
{
	return _right;
}

template <typename T>
void Rect<T>::set_coords(T left, T top, T right, T bottom) noexcept
{
	_left = left;
	_top = top;
	_right = right;
	_bottom = bottom;
}

template <typename T>
void Rect<T>::set_height(T height) noexcept
{
	_bottom = _top + height;
}

template <typename T>
void Rect<T>::set_left(T left) noexcept
{
	_right += left - _left;
	_left = left;
}

template <typename T>
void Rect<T>::set_size(T width, T height) noexcept
{
	_right = _left + width;
	_bottom = _top + height;
}

template <typename T>
void Rect<T>::set_size(const Vector2<T> &size) noexcept
{
	_right = _left + size.x;
	_bottom = _top + size.y;
}

template <typename T>
void Rect<T>::set_top(T top) noexcept
{
	_bottom += top - _top;
	_top = top;
}

template <typename T>
void Rect<T>::set_top_left(T left, T top) noexcept
{
	_right += left - _left;
	_left = left;
	_bottom += top - _top;
	_top = top;
}

template <typename T>
void Rect<T>::set_top_left(const Vector2<T> &top_left) noexcept
{
	_right += top_left.x - _left;
	_left = top_left.x;
	_bottom += top_left.y - _top;
	_top = top_left.y;
}

template <typename T>
void Rect<T>::set_width(T width) noexcept
{
	_right = _left + width;
}

template <typename T>
Vector2<T> Rect<T>::size() const noexcept
{
	return Vector2<T>(_right - _left, _bottom - _top);
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
	return Vector2<T>(_right, _top);
}

template <typename T>
bool Rect<T>::contains(const Vector2<T> &point) const noexcept
{
	return _left <= point.x && point.x < _right
		&& _top <= point.y && point.y < _bottom;
}

template <typename T>
bool Rect<T>::contains(const Rect &rect) const noexcept
{
	return _left <= rect._left && rect._right <= _right
		&& _top <= rect._top && rect._bottom <= _bottom;
}

template <typename T>
bool Rect<T>::fast_contains(const Vector2<T> &point) const noexcept
{
	return ((point.x - _left) ^ (point.x - _right)) < 0
		&& ((point.y - _top) ^ (point.y - _bottom)) < 0;
}

template <typename T>
bool Rect<T>::fast_intersects(const Rect &rect) const noexcept
{
	return (_left - rect._right) * (_right - rect._left) < 0
		&& (_top - rect._bottom) * (_bottom - rect._top) < 0;
}

template <typename T>
bool Rect<T>::fastest_intersects(const Rect &rect) const noexcept
{
	return ((_left - rect._right) ^ (rect._left - _right)) >= 0
		&& ((_top - rect._bottom) ^ (rect._top - _bottom)) >= 0;
}

template <typename T>
bool Rect<T>::intersects(const Rect &rect) const noexcept
{
	return _left < rect._right && rect._left < _right
		&& _top < rect._bottom && rect._top < _bottom;
}

template <typename T>
T Rect<T>::width() const noexcept
{
	return _right - _left;
}

template <typename T>
Rect<T> Rect<T>::from_coords(T left, T top, T right, T bottom) noexcept
{
	return Rect(left, top, right - left, bottom - top);
}

} // namespace Yttrium

#endif // __Y_RECT_H
