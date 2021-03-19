// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/math/size.h>

#include <algorithm>

namespace Yt
{
	/// Integral rectangle.
	class Rect
	{
	public:
		constexpr Rect() noexcept = default;
		constexpr Rect(const Point& top_left, const Point& bottom_right) noexcept
			: _left{ top_left._x }, _top{ top_left._y }, _right{ bottom_right._x }, _bottom{ bottom_right._y } {}
		constexpr Rect(const Point& top_left, const Size& s) noexcept
			: _left{ top_left._x }, _top{ top_left._y }, _right{ _left + s._width }, _bottom{ _top + s._height } {}
		explicit constexpr Rect(const Size& s) noexcept
			: _right{ s._width }, _bottom{ s._height } {}

		constexpr int bottom() const noexcept { return _bottom; }
		constexpr Point bottom_left() const noexcept { return { _left, _bottom }; }
		constexpr Point bottom_right() const noexcept { return { _right, _bottom }; }
		constexpr int height() const noexcept { return _bottom - _top; }
		constexpr int left() const noexcept { return _left; }
		constexpr int right() const noexcept { return _right; }
		constexpr Size size() const noexcept { return { width(), height() }; }
		constexpr int top() const noexcept { return _top; }
		constexpr Point top_left() const noexcept { return { _left, _top }; }
		constexpr Point top_right() const noexcept { return { _right, _top }; }
		constexpr int width() const noexcept { return _right - _left; }

		///
		constexpr Point center() const noexcept { return { (_left + _right) / 2, (_top + _bottom) / 2 }; }

		///
		constexpr bool is_empty() const noexcept
		{
			return _left >= _right || _top >= _bottom;
		}

		///
		constexpr Point bound(const Point& p) const noexcept
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
			return { x, y };
		}

		///
		constexpr Rect centered_at(const Rect& rect) const noexcept
		{
			return {
				{ (rect._right + rect._left - width()) / 2, (rect._bottom + rect._top - height()) / 2 },
				size()
			};
		}

		///
		constexpr bool contains(const Point& p) const noexcept
		{
			return _left <= p._x && p._x < _right
				&& _top <= p._y && p._y < _bottom;
		}

		///
		constexpr bool contains(const Rect& rect) const noexcept
		{
			return _left <= rect._left && rect._right <= _right
				&& _top <= rect._top && rect._bottom <= _bottom;
		}

		///
		constexpr bool contains_fast(const Point& p) const noexcept
		{
			return ((p._x - _left) ^ (p._x - _right)) < 0
				&& ((p._y - _top) ^ (p._y - _bottom)) < 0;
		}

		///
		constexpr Rect intersected(const Rect& rect) const noexcept
		{
			return {
				{ std::max(_left, rect._left), std::max(_top, rect._top) },
				Point{ std::min(_right, rect._right), std::min(_bottom, rect._bottom) }
			};
		}

		/// Find whether the rects intersect.
		/// \note This function reports no intersection of a null rect with itself,
		/// though it does report an intersection for a null rect with a non-null rect
		/// if the point of the null rect lies inside the non-null rect.
		constexpr bool intersects(const Rect& rect) const noexcept
		{
			return _left < rect._right && rect._left < _right
				&& _top < rect._bottom && rect._top < _bottom;
		}

		/// Find whether the rects intersect.
		constexpr bool intersects_fast(const Rect& rect) const noexcept
		{
			return (_left - rect._right) * (_right - rect._left) < 0
				&& (_top - rect._bottom) * (_bottom - rect._top) < 0;
		}

		/// Find whether the rects intersect.
		/// \note The function assumes <tt>left <= right</tt> and <tt>top <= bottom</tt>.
		/// \note The function assumes that the subtraction result of any two coordinates along the same axis is valid.
		/// \note The function reports an intersection of a null rect with itself
		/// or with a non-null rect if the point of the null rect lies inside the non-null rect.
		constexpr bool intersects_fastest(const Rect& rect) const noexcept
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
		constexpr RectF() noexcept = default;
		constexpr RectF(const Vector2& top_left, const Vector2& bottom_right) noexcept
			: _left{ top_left.x }, _top{ top_left.y }, _right{ bottom_right.x }, _bottom{ bottom_right.y } {}
		constexpr RectF(const Vector2& top_left, const SizeF& s) noexcept
			: _left{ top_left.x }, _top{ top_left.y }, _right{ _left + s._width }, _bottom{ _top + s._height } {}
		constexpr explicit RectF(const SizeF& s) noexcept
			: _right{ static_cast<float>(s._width) }, _bottom{ static_cast<float>(s._height) } {}
		explicit RectF(const Rect& r) noexcept
			: _left{ static_cast<float>(r.left()) }, _top{ static_cast<float>(r.top()) }, _right{ static_cast<float>(r.right()) }, _bottom{ static_cast<float>(r.bottom()) } {}

		constexpr float bottom() const noexcept { return _bottom; }
		constexpr Vector2 bottom_left() const noexcept { return { _left, _bottom }; }
		constexpr Vector2 bottom_right() const noexcept { return { _right, _bottom }; }
		constexpr float height() const noexcept { return _bottom - _top; }
		constexpr float left() const noexcept { return _left; }
		constexpr float right() const noexcept { return _right; }
		constexpr SizeF size() const noexcept { return { width(), height() }; }
		constexpr float top() const noexcept { return _top; }
		constexpr Vector2 top_left() const noexcept { return { _left, _top }; }
		constexpr Vector2 top_right() const noexcept { return { _right, _top }; }
		constexpr float width() const noexcept { return _right - _left; }

		///
		constexpr Vector2 center() const noexcept { return { (_left + _right) / 2, (_top + _bottom) / 2 }; }

		///
		constexpr bool contains(const Vector2& v) const noexcept { return _left <= v.x && v.x < _right && _top <= v.y && v.y < _bottom; }

		///
		Vector2 bound(const Vector2& p) const noexcept
		{
			auto x = p.x;
			if (x < _left)
				x = _left;
			else if (x > _right - 1)
				x = _right - 1;
			auto y = p.y;
			if (y < _top)
				y = _top;
			else if (y >= _bottom - 1)
				y = _bottom - 1;
			return { x, y };
		}

		constexpr void set_right(float right) noexcept { _right = right; }
		constexpr void set_width(float width) noexcept { _right = _left + width; }

	private:
		float _left = 0;
		float _top = 0;
		float _right = 0;
		float _bottom = 0;
	};

	constexpr bool operator==(const RectF& a, const RectF& b) noexcept
	{
		return a.left() == b.left() && a.top() == b.top() && a.right() == b.right() && a.bottom() == b.bottom();
	}

	constexpr RectF operator/(const RectF& rect, float s) noexcept { return { rect.top_left() / s, rect.bottom_right() / s }; }
	constexpr RectF operator/(const RectF& rect, const Vector2& v) noexcept { return { rect.top_left() / v, rect.bottom_right() / v }; }
}
