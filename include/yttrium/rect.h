/// \file
/// \brief Rectangles.

#ifndef __Y_RECT_H
#define __Y_RECT_H

#include <yttrium/vector.h>

namespace Yttrium
{

/// Rectangle.

template <typename T>
class Rect_
{
public:

	///

	explicit Rect_(T left = 0, T top = 0, T width = 0, T height = 0) noexcept
		: _left(left)
		, _top(top)
		, _right(left + width)
		, _bottom(top + height)
	{
	}

	///

	explicit Rect_(const Vector2<T>& size) noexcept
		: _left(0)
		, _top(0)
		, _right(size.x)
		, _bottom(size.y)
	{
	}

	///

	Rect_(const Vector2<T>& corner, const Vector2<T>& size) noexcept
		: _left(corner.x)
		, _top(corner.y)
		, _right(corner.x + size.x)
		, _bottom(corner.y + size.y)
	{
	}

	///

	template <typename U>
	explicit Rect_(const Rect_<U>& rect) noexcept
		: _left(rect.left())
		, _top(rect.top())
		, _right(rect.right())
		, _bottom(rect.bottom())
	{
	}

public:

	///

	T bottom() const noexcept
	{
		return _bottom;
	}

	///

	Vector2<T> bottom_left() const noexcept
	{
		return Vector2<T>(_left, _bottom);
	}

	///

	Vector2<T> bottom_right() const noexcept
	{
		return Vector2<T>(_right, _bottom);
	}

	///

	Vector2<T> center() const noexcept
	{
		return Vector2<T>((_left + _right) / 2, (_top + _bottom) / 2);
	}

	///

	T height() const noexcept
	{
		return _bottom - _top;
	}

	///

	bool is_empty() const noexcept
	{
		return _left >= _right || _top >= _bottom;
	}

	///

	T left() const noexcept
	{
		return _left;
	}

	///

	T right() const noexcept
	{
		return _right;
	}

	///

	void set_coords(T left, T top, T right, T bottom) noexcept
	{
		_left = left;
		_top = top;
		_right = right;
		_bottom = bottom;
	}

	///

	void set_height(T height) noexcept
	{
		_bottom = _top + height;
	}

	///

	void set_left(T left) noexcept
	{
		_right += left - _left;
		_left = left;
	}

	///

	void set_size(T width, T height) noexcept
	{
		_right = _left + width;
		_bottom = _top + height;
	}

	///

	void set_size(const Vector2<T>& size) noexcept
	{
		_right = _left + size.x;
		_bottom = _top + size.y;
	}

	///

	void set_top(T top) noexcept
	{
		_bottom += top - _top;
		_top = top;
	}

	///

	void set_top_left(T left, T top) noexcept
	{
		_right += left - _left;
		_left = left;
		_bottom += top - _top;
		_top = top;
	}

	///

	void set_top_left(const Vector2<T>& top_left) noexcept
	{
		_right += top_left.x - _left;
		_left = top_left.x;
		_bottom += top_left.y - _top;
		_top = top_left.y;
	}

	///

	void set_width(T width) noexcept
	{
		_right = _left + width;
	}

	///

	Vector2<T> size() const noexcept
	{
		return Vector2<T>(_right - _left, _bottom - _top);
	}

	///

	T top() const noexcept
	{
		return _top;
	}

	///

	Vector2<T> top_left() const noexcept
	{
		return Vector2<T>(_left, _top);
	}

	///

	Vector2<T> top_right() const noexcept
	{
		return Vector2<T>(_right, _top);
	}

	///

	T width() const noexcept
	{
		return _right - _left;
	}

public:

	///

	bool contains(const Vector2<T>& point) const noexcept
	{
		return _left <= point.x && point.x < _right
			&& _top <= point.y && point.y < _bottom;
	}

	///

	bool contains(const Rect_& rect) const noexcept
	{
		return _left <= rect._left && rect._right <= _right
			&& _top <= rect._top && rect._bottom <= _bottom;
	}

	/// Find whether the rects intersect.

	bool fast_intersects(const Rect_& rect) const noexcept
	{
		return (_left - rect._right) * (_right - rect._left) < 0
			&& (_top - rect._bottom) * (_bottom - rect._top) < 0;
	}

	/// Find whether the rects intersect.
	/// \param rect
	/// \return \c true if this rect intersects \a rect.
	/// \note This function reports no intersection of a null rect with itself,
	/// though it does report an intersection for a null rect with a non-null rect
	/// if the point of the null rect lies inside the non-null rect.

	bool intersects(const Rect_& rect) const noexcept
	{
		return _left < rect._right && rect._left < _right
			&& _top < rect._bottom && rect._top < _bottom;
	}

public:

	///

	static Rect_ from_coords(T left, T top, T right, T bottom) noexcept
	{
		return Rect_(left, top, right - left, bottom - top);
	}

protected:

	T _left;
	T _top;
	T _right;
	T _bottom;
};

///

class Rect: public Rect_<int>
{
public:

	///

	explicit Rect(int left = 0, int top = 0, int width = 0, int height = 0) noexcept
		: Rect_(left, top, width, height)
	{
	}

	///

	explicit Rect(const Vector2<int>& size) noexcept
		: Rect_(size)
	{
	}

	///

	Rect(const Vector2<int> &corner, const Vector2<int> &size) noexcept
		: Rect_(corner, size)
	{
	}

	///

	template <typename U>
	explicit Rect(const Rect_<U> &rect) noexcept
		: Rect_(rect)
	{
	}

public:

	///

	bool fast_contains(const Vector2<int>& point) const noexcept
	{
		return ((point.x - _left) ^ (point.x - _right)) < 0
			&& ((point.y - _top) ^ (point.y - _bottom)) < 0;
	}

	/// Find whether the rects intersect.
	/// \param rect
	/// \return \c true if this rect intersects \a rect.
	/// \note The function assumes <tt>left <= right</tt> and <tt>top <= bottom</tt>.
	/// \note The function assumes that the subtraction result of any two coordinates along the same axis is valid.
	/// \note The function reports an intersection of a null rect with itself
	/// or with a non-null rect if the point of the null rect lies inside the non-null rect.

	bool fastest_intersects(const Rect& rect) const noexcept
	{
		return ((_left - rect._right) ^ (rect._left - _right)) >= 0
			&& ((_top - rect._bottom) ^ (rect._top - _bottom)) >= 0;
	}
};

} // namespace Yttrium

#endif // __Y_RECT_H
