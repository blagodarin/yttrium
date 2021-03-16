// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/math/color.h>
#include <yttrium/math/rect.h>
#include <yttrium/math/vector.h>

namespace Yt
{
	class RectF;

	class GuiLayout
	{
	public:
		enum class Axis
		{
			X,
			Y,
		};

		constexpr explicit GuiLayout(const RectF& rect) noexcept
			: _rect{ rect } {}
		constexpr explicit GuiLayout(const SizeF& size) noexcept
			: _rect{ size } {}

		RectF add(float width, float height) noexcept;
		void fromBottomLeft(Axis, float padding = 0) noexcept;
		void fromBottomRight(Axis, float padding = 0) noexcept;
		constexpr void scaleForHeight(float height) noexcept { _scaling = _rect.height() / height; }
		constexpr void scaleForWidth(float width) noexcept { _scaling = _rect.width() / width; }
		constexpr void setSpacing(float spacing) noexcept { _spacing = spacing; }

	private:
		const RectF _rect;
		float _scaling = 1;
		Vector2 _direction{ 1, 1 };
		Vector2 _position{ 0, 0 };
		Axis _axis = Axis::X;
		float _spacing = 0;
	};
}
