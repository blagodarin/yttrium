// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/geometry/size.h>

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

	constexpr bool operator==(const Rect& a, const Rect& b) noexcept { return a.left() == b.left() && a.top() == b.top() && a.right() == b.right() && a.bottom() == b.bottom(); }

	/// Floating-point rectangle.
	class RectF
	{
	public:
		float _left = 0;
		float _top = 0;
		float _right = 0;
		float _bottom = 0;

		constexpr RectF() noexcept = default;
		constexpr RectF(const Vector2& topLeft, const Vector2& bottomRight) noexcept
			: _left{ topLeft.x }, _top{ topLeft.y }, _right{ bottomRight.x }, _bottom{ bottomRight.y } {}
		constexpr RectF(const Vector2& topLeft, const SizeF& size) noexcept
			: _left{ topLeft.x }, _top{ topLeft.y }, _right{ _left + size._width }, _bottom{ _top + size._height } {}
		constexpr explicit RectF(const SizeF& size) noexcept
			: _right{ size._width }, _bottom{ size._height } {}
		explicit RectF(const Rect& r) noexcept
			: _left{ static_cast<float>(r.left()) }, _top{ static_cast<float>(r.top()) }, _right{ static_cast<float>(r.right()) }, _bottom{ static_cast<float>(r.bottom()) } {}

		[[nodiscard]] constexpr float bottom() const noexcept { return _bottom; }
		[[nodiscard]] constexpr Vector2 bottomCenter() const noexcept { return { (_left + _right) / 2, _bottom }; }
		[[nodiscard]] constexpr Vector2 bottomLeft() const noexcept { return { _left, _bottom }; }
		[[nodiscard]] constexpr Vector2 bottomRight() const noexcept { return { _right, _bottom }; }
		[[nodiscard]] constexpr Vector2 center() const noexcept { return { (_left + _right) / 2, (_top + _bottom) / 2 }; }
		[[nodiscard]] constexpr Vector2 centerLeft() const noexcept { return { _left, (_top + _bottom) / 2 }; }
		[[nodiscard]] constexpr Vector2 centerRight() const noexcept { return { _right, (_top + _bottom) / 2 }; }
		[[nodiscard]] constexpr bool contains(const Vector2& point) const noexcept { return _left <= point.x && point.x < _right && _top <= point.y && point.y < _bottom; }
		[[nodiscard]] constexpr bool empty() const noexcept { return _left >= _right || _top >= _bottom; }
		[[nodiscard]] constexpr float height() const noexcept { return _bottom - _top; }
		[[nodiscard]] constexpr float left() const noexcept { return _left; }
		[[nodiscard]] constexpr bool null() const noexcept { return _left == _right && _top == _bottom; }
		[[nodiscard]] constexpr float right() const noexcept { return _right; }
		constexpr void setHeight(float value) noexcept { _bottom = _top + value; }
		constexpr void setWidth(float value) noexcept { _right = _left + value; }
		[[nodiscard]] constexpr SizeF size() const noexcept { return { _right - _left, _bottom - _top }; }
		[[nodiscard]] constexpr float top() const noexcept { return _top; }
		[[nodiscard]] constexpr Vector2 topCenter() const noexcept { return { (_left + _right) / 2, _top }; }
		[[nodiscard]] constexpr Vector2 topLeft() const noexcept { return { _left, _top }; }
		[[nodiscard]] constexpr Vector2 topRight() const noexcept { return { _right, _top }; }
		[[nodiscard]] constexpr float width() const noexcept { return _right - _left; }

		///
		[[nodiscard]] Vector2 bound(const Vector2& point) const noexcept
		{
			auto x = point.x;
			if (x < _left)
				x = _left;
			else if (x > _right - 1)
				x = _right - 1;
			auto y = point.y;
			if (y < _top)
				y = _top;
			else if (y >= _bottom - 1)
				y = _bottom - 1;
			return { x, y };
		}
	};

	[[nodiscard]] constexpr bool operator==(const RectF& a, const RectF& b) noexcept { return a.left() == b.left() && a.top() == b.top() && a.right() == b.right() && a.bottom() == b.bottom(); }

	[[nodiscard]] constexpr RectF operator+(const RectF& rect, float value) noexcept { return { rect.topLeft() + value, rect.bottomRight() + value }; }
	[[nodiscard]] constexpr RectF operator+(const RectF& rect, const Vector2& value) noexcept { return { value + rect.topLeft(), value + rect.bottomRight() }; }

	[[nodiscard]] constexpr RectF operator-(const RectF& rect, float value) noexcept { return { rect.topLeft() - value, rect.bottomRight() - value }; }
	[[nodiscard]] constexpr RectF operator-(const RectF& rect, const Vector2& value) noexcept { return { rect.topLeft() - value, rect.bottomRight() - value }; }

	[[nodiscard]] constexpr RectF operator*(const RectF& rect, float value) noexcept { return { rect.topLeft() * value, rect.bottomRight() * value }; }
	[[nodiscard]] constexpr RectF operator*(const RectF& rect, const Vector2& value) noexcept { return { rect.topLeft() * value, rect.bottomRight() * value }; }

	[[nodiscard]] constexpr RectF operator/(const RectF& rect, float value) noexcept { return { rect.topLeft() / value, rect.bottomRight() / value }; }
	[[nodiscard]] constexpr RectF operator/(const RectF& rect, const Vector2& value) noexcept { return { rect.topLeft() / value, rect.bottomRight() / value }; }
}
