/// \file
/// \brief Rectangles.

#ifndef __Y_RECT_H
#define __Y_RECT_H

#include <yttrium/point.h>
#include <yttrium/size.h>
#include <yttrium/vector.h>

namespace Yttrium
{
	///
	class Rect
	{
	public:

		///
		explicit Rect(int left = 0, int top = 0, int width = 0, int height = 0)
			: _left(left)
			, _top(top)
			, _right(left + width)
			, _bottom(top + height)
		{
		}

		///
		Rect(const Point& top_left, const Size& size)
			: Rect(top_left.x, top_left.y, size.width, size.height)
		{
		}

		///
		explicit Rect(const Size& size)
			: Rect(0, 0, size.width, size.height)
		{
		}

		///
		int bottom() const { return _bottom; }

		///
		Point bottom_left() const { return Point(_left, _bottom); }

		///
		Point bottom_right() const { return Point(_right, _bottom); }

		///
		bool contains(const Point& point) const
		{
			return _left <= point.x && point.x < _right
				&& _top <= point.y && point.y < _bottom;
		}

		///
		bool contains(const Rect& rect) const
		{
			return _left <= rect._left && rect._right <= _right
				&& _top <= rect._top && rect._bottom <= _bottom;
		}

		///
		bool contains_fast(const Point& point) const
		{
			return ((point.x - _left) ^ (point.x - _right)) < 0
				&& ((point.y - _top) ^ (point.y - _bottom)) < 0;
		}

		///
		int height() const { return _bottom - _top; }

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

		///
		bool is_empty() const { return _left >= _right || _top >= _bottom; }

		///
		int left() const { return _left; }

		///
		int right() const { return _right; }

		///
		void set_coords(int left, int top, int right, int bottom)
		{
			_left = left;
			_top = top;
			_right = right;
			_bottom = bottom;
		}

		///
		void set_height(int height) { _bottom = _top + height; }

		///
		void set_left(int left)
		{
			_right += left - _left;
			_left = left;
		}

		///
		void set_size(int width, int height)
		{
			_right = _left + width;
			_bottom = _top + height;
		}

		///
		void set_size(const Size& size) { set_size(size.width, size.height); }

		///
		void set_top(int top)
		{
			_bottom += top - _top;
			_top = top;
		}

		///
		void set_top_left(int left, int top)
		{
			_right += left - _left;
			_left = left;
			_bottom += top - _top;
			_top = top;
		}

		///
		void set_top_left(const Point& top_left) { set_top_left(top_left.x, top_left.y); }

		///
		void set_width(int width) { _right = _left + width; }

		///
		Size size() const { return Size(_right - _left, _bottom - _top); }

		///
		int top() const { return _top; }

		///
		Point top_left() const { return Point(_left, _top); }

		///
		Point top_right() const { return Point(_right, _top); }

		///
		int width() const { return _right - _left; }

	public:

		///
		static Rect from_coords(int left, int top, int right, int bottom)
		{
			return Rect(left, top, right - left, bottom - top);
		}

	private:

		int _left;
		int _top;
		int _right;
		int _bottom;
	};

	///
	class RectF
	{
	public:

		///
		explicit RectF(float left = 0, float top = 0, float width = 0, float height = 0)
			: _left(left)
			, _top(top)
			, _right(left + width)
			, _bottom(top + height)
		{
		}

		///
		RectF(const Vector2& top_left, const Vector2& size)
			: RectF(top_left.x, top_left.y, size.x, size.y)
		{
		}

		///
		explicit RectF(const Rect& rect)
			: _left(rect.left())
			, _top(rect.top())
			, _right(rect.right())
			, _bottom(rect.bottom())
		{
		}

		///
		float bottom() const { return _bottom; }

		///
		Vector2 bottom_left() const { return Vector2(_left, _bottom); }

		///
		Vector2 bottom_right() const { return Vector2(_right, _bottom); }

		///
		Vector2 center() const { return Vector2((_left + _right) / 2, (_top + _bottom) / 2); }

		///
		bool contains(const Vector2& point) const
		{
			return _left <= point.x && point.x < _right
				&& _top <= point.y && point.y < _bottom;
		}

		///
		float height() const { return _bottom - _top; }

		/// Find whether the rects intersect.
		/// \param rect
		/// \return \c true if this rect intersects \a rect.
		/// \note This function reports no intersection of a null rect with itself,
		/// though it does report an intersection for a null rect with a non-null rect
		/// if the point of the null rect lies inside the non-null rect.
		bool intersects(const RectF& rect) const
		{
			return _left < rect._right && rect._left < _right
				&& _top < rect._bottom && rect._top < _bottom;
		}

		/// Find whether the rects intersect.
		bool intersects_fast(const RectF& rect) const
		{
			return (_left - rect._right) * (_right - rect._left) < 0
				&& (_top - rect._bottom) * (_bottom - rect._top) < 0;
		}

		///
		bool is_empty() const { return _left >= _right || _top >= _bottom; }

		///
		float left() const { return _left; }

		///
		float right() const { return _right; }

		///
		void set_coords(float left, float top, float right, float bottom)
		{
			_left = left;
			_top = top;
			_right = right;
			_bottom = bottom;
		}

		///
		void set_height(float height) { _bottom = _top + height; }

		///
		void set_left(float left)
		{
			_right += left - _left;
			_left = left;
		}

		///
		void set_size(float width, float height)
		{
			_right = _left + width;
			_bottom = _top + height;
		}

		///
		void set_size(const Vector2& size) { set_size(size.x, size.y); }

		///
		void set_top(float top)
		{
			_bottom += top - _top;
			_top = top;
		}

		///
		void set_top_left(float left, float top)
		{
			_right += left - _left;
			_left = left;
			_bottom += top - _top;
			_top = top;
		}

		///
		void set_top_left(const Vector2& top_left) { set_top_left(top_left.x, top_left.y); }

		///
		void set_width(float width) { _right = _left + width; }

		///
		Vector2 size() const { return Vector2(_right - _left, _bottom - _top); }

		///
		float top() const { return _top; }

		///
		Vector2 top_left() const { return Vector2(_left, _top); }

		///
		Vector2 top_right() const { return Vector2(_right, _top); }

		///
		float width() const { return _right - _left; }

	public:

		///
		static RectF from_coords(float left, float top, float right, float bottom)
		{
			return RectF(left, top, right - left, bottom - top);
		}

	private:

		float _left;
		float _top;
		float _right;
		float _bottom;
	};
}

#endif // __Y_RECT_H
