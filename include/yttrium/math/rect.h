#ifndef _include_yttrium_math_rect_h_
#define _include_yttrium_math_rect_h_

#include <yttrium/math/size.h>

#include <algorithm>

namespace Yttrium
{
	/// Integral rectangle.
	class Rect
	{
	public:
		Rect() = default;
		Rect(const Point& top_left, const Point& bottom_right) : _left{top_left._x}, _top{top_left._y}, _right{bottom_right._x}, _bottom{bottom_right._y} {}
		Rect(const Point& top_left, const Size& s) : _left{top_left._x}, _top{top_left._y}, _right{_left + s._width}, _bottom{_top + s._height} {}
		explicit Rect(const Size& s) : _right{s._width}, _bottom{s._height} {}

		int bottom() const { return _bottom; }
		Point bottom_left() const { return {_left, _bottom}; }
		Point bottom_right() const { return {_right, _bottom}; }
		int height() const { return _bottom - _top; }
		int left() const { return _left; }
		int right() const { return _right; }
		Size size() const { return {width(), height()}; }
		int top() const { return _top; }
		Point top_left() const { return {_left, _top}; }
		Point top_right() const { return {_right, _top}; }
		int width() const { return _right - _left; }

		///
		Point center() const { return {(_left + _right) / 2, (_top + _bottom) / 2}; }

		///
		bool is_empty() const
		{
			return _left >= _right || _top >= _bottom;
		}

		///
		Point bound(const Point& p) const
		{
			auto x = p._x;
			if (x < _left)
				x = _left;
			else if (x >= _right)
				x = _right - 1;
			auto y = p._y;
			if (y < _top)
				y = _top;
			else if (y >= _bottom)
				y = _bottom - 1;
			return {x, y};
		}

		///
		Rect centered_at(const Rect& rect) const
		{
			return
			{
				{(rect._right + rect._left - width()) / 2, (rect._bottom + rect._top - height()) / 2},
				size()
			};
		}

		///
		bool contains(const Point& p) const
		{
			return _left <= p._x && p._x < _right
				&& _top <= p._y && p._y < _bottom;
		}

		///
		bool contains(const Rect& rect) const
		{
			return _left <= rect._left && rect._right <= _right
				&& _top <= rect._top && rect._bottom <= _bottom;
		}

		///
		bool contains_fast(const Point& p) const
		{
			return ((p._x - _left) ^ (p._x - _right)) < 0
				&& ((p._y - _top) ^ (p._y - _bottom)) < 0;
		}

		///
		Rect intersected(const Rect& rect) const
		{
			return
			{
				{std::max(_left, rect._left), std::max(_top, rect._top)},
				Point{std::min(_right, rect._right), std::min(_bottom, rect._bottom)}
			};
		}

		/// Find whether the rects intersect.
		/// \param rect
		/// \return \c true if this rect intersects \a rect.
		/// \note This function reports no intersection of a null rect with itself,
		/// though it does report an intersection for a null rect with a non-null rect
		/// if the point of the null rect lies inside the non-null rect.
		bool intersects(const Rect& rect) const
		{
			return _left < rect._right && rect._left < _right
				&& _top < rect._bottom && rect._top < _bottom;
		}

		/// Find whether the rects intersect.
		bool intersects_fast(const Rect& rect) const
		{
			return (_left - rect._right) * (_right - rect._left) < 0
				&& (_top - rect._bottom) * (_bottom - rect._top) < 0;
		}

		/// Find whether the rects intersect.
		/// \param rect
		/// \return \c true if this rect intersects \a rect.
		/// \note The function assumes <tt>left <= right</tt> and <tt>top <= bottom</tt>.
		/// \note The function assumes that the subtraction result of any two coordinates along the same axis is valid.
		/// \note The function reports an intersection of a null rect with itself
		/// or with a non-null rect if the point of the null rect lies inside the non-null rect.
		bool intersects_fastest(const Rect& rect) const
		{
			return ((_left - rect._right) ^ (rect._left - _right)) >= 0
				&& ((_top - rect._bottom) ^ (rect._top - _bottom)) >= 0;
		}

	private:
		int _left = 0;
		int _top = 0;
		int _right = 0;
		int _bottom = 0;
	};

	inline bool operator==(const Rect& lhs, const Rect& rhs)
	{
		return lhs.left() == rhs.left() && lhs.top() == rhs.top()
			&& lhs.right() == rhs.right() && lhs.bottom() == rhs.bottom();
	}

	inline bool operator!=(const Rect& lhs, const Rect& rhs)
	{
		return !(lhs == rhs);
	}

	/// Floating-point rectangle.
	class RectF
	{
	public:
		RectF() = default;
		RectF(const Vector2& top_left, const Vector2& bottom_right) : _left{top_left.x}, _top{top_left.y}, _right{bottom_right.x}, _bottom{bottom_right.y} {}
		RectF(const Vector2& top_left, const SizeF& s) : _left{top_left.x}, _top{top_left.y}, _right{_left + s._width}, _bottom{_top + s._height} {}
		explicit RectF(const SizeF& s) : _right{static_cast<float>(s._width)}, _bottom{static_cast<float>(s._height)} {}
		explicit RectF(const Rect& r) : _left{static_cast<float>(r.left())}, _top{static_cast<float>(r.top())}, _right{static_cast<float>(r.right())}, _bottom{static_cast<float>(r.bottom())} {}

		float bottom() const { return _bottom; }
		Vector2 bottom_left() const { return {_left, _bottom}; }
		Vector2 bottom_right() const { return {_right, _bottom}; }
		float height() const { return _bottom - _top; }
		float left() const { return _left; }
		float right() const { return _right; }
		SizeF size() const { return {width(), height()}; }
		float top() const { return _top; }
		Vector2 top_left() const { return {_left, _top}; }
		Vector2 top_right() const { return {_right, _top}; }
		float width() const { return _right - _left; }

		///
		Vector2 center() const { return {(_left + _right) / 2, (_top + _bottom) / 2}; }

		///
		bool contains(const Vector2& v) const { return _left <= v.x && v.x < _right && _top <= v.y && v.y < _bottom; }

	private:
		float _left = 0;
		float _top = 0;
		float _right = 0;
		float _bottom = 0;
	};

	inline bool operator==(const RectF& lhs, const RectF& rhs)
	{
		return lhs.left() == rhs.left() && lhs.top() == rhs.top()
			&& lhs.right() == rhs.right() && lhs.bottom() == rhs.bottom();
	}

	inline bool operator!=(const RectF& lhs, const RectF& rhs)
	{
		return !(lhs == rhs);
	}

	inline RectF operator/(const RectF& rect, float s) { return {rect.top_left() / s, rect.bottom_right() / s}; }
	inline RectF operator/(const RectF& rect, const Vector2& v) { return {rect.top_left() / v, rect.bottom_right() / v}; }
}

#endif
