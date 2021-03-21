// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/math/rect.h>

namespace Yt
{
	class GuiLayout
	{
	public:
		enum class Axis
		{
			X,
			Y,
		};

		explicit GuiLayout(const RectF& rect) noexcept
			: _rect{ rect } {}

		RectF add(const SizeF& = {}) noexcept;
		void fromBottomCenter(float padding = 0) noexcept { fromPoint({ _size._width / 2, _size._height }, { 0, -1 }, Axis::Y, padding); }
		void fromBottomLeft(Axis axis, float padding = 0) noexcept { fromPoint({ 0, _size._height }, { 1, -1 }, axis, padding); }
		void fromBottomRight(Axis axis, float padding = 0) noexcept { fromPoint({ _size._width, _size._height }, { -1, -1 }, axis, padding); }
		void fromPoint(const Vector2& point, const Vector2& direction, Axis axis, float padding = 0) noexcept;
		void fromTopCenter(float padding = 0) noexcept { fromPoint({ _size._width / 2, 0 }, { 0, 1 }, Axis::Y, padding); }
		void fromTopLeft(Axis axis, float padding = 0) noexcept { fromPoint({ 0, 0 }, { 1, 1 }, axis, padding); }
		void fromTopRight(Axis axis, float padding = 0) noexcept { fromPoint({ _size._width, 0 }, { -1, 1 }, axis, padding); }
		void mapToCenter(const SizeF&) noexcept;
		void scaleForHeight(float height) noexcept;
		void scaleForWidth(float width) noexcept;
		constexpr void setAxis(Axis axis) noexcept { _axis = axis; }
		constexpr void setSize(const SizeF& size) noexcept { _defaultSize = size; }
		constexpr void setSpacing(float spacing) noexcept { _spacing = spacing; }
		[[nodiscard]] constexpr Vector2 transform(const Vector2& point) const noexcept { return point * _scaling + _offset; }
		[[nodiscard]] constexpr RectF transform(const RectF& rect) const noexcept { return rect * _scaling + _offset; }

	private:
		const RectF _rect;
		Vector2 _offset{ 0, 0 };
		SizeF _size = _rect.size();
		float _scaling = 1;
		Vector2 _direction{ 1, 1 };
		Vector2 _position{ 0, 0 };
		Axis _axis = Axis::X;
		float _spacing = 0;
		SizeF _defaultSize;
	};
}
