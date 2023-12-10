// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/geometry/rect.h>

namespace Yt
{
	class GuiFrame;

	class GuiLayout
	{
	public:
		struct Center
		{
			const float _width;
			const float _height;
		};

		struct Height
		{
			const float _height;
		};

		struct Width
		{
			const float _width;
		};

		enum class Axis
		{
			X,
			Y,
		};

		explicit GuiLayout(GuiFrame&) noexcept;
		GuiLayout(GuiFrame&, const Center&) noexcept;
		GuiLayout(GuiFrame&, const Height&) noexcept;
		GuiLayout(GuiFrame&, const Width&) noexcept;
		~GuiLayout() noexcept;

		RectF add(const SizeF& = {}) noexcept;
		void fromBottomCenter(float padding = 0) noexcept { fromPoint({ _size._width / 2, _size._height }, { 0, -1 }, Axis::Y, padding); }
		void fromBottomLeft(Axis axis, float padding = 0) noexcept { fromPoint({ 0, _size._height }, { 1, -1 }, axis, padding); }
		void fromBottomRight(Axis axis, float padding = 0) noexcept { fromPoint({ _size._width, _size._height }, { -1, -1 }, axis, padding); }
		void fromPoint(const seir::Vec2& point, const seir::Vec2& direction, Axis axis, float padding = 0) noexcept;
		void fromTopCenter(float padding = 0) noexcept { fromPoint({ _size._width / 2, 0 }, { 0, 1 }, Axis::Y, padding); }
		void fromTopLeft(Axis axis, float padding = 0) noexcept { fromPoint({ 0, 0 }, { 1, 1 }, axis, padding); }
		void fromTopRight(Axis axis, float padding = 0) noexcept { fromPoint({ _size._width, 0 }, { -1, 1 }, axis, padding); }
		[[nodiscard]] RectF map(const RectF&) const noexcept;
		constexpr void setAxis(Axis axis) noexcept { _axis = axis; }
		constexpr void setSize(const SizeF& size) noexcept { _defaultSize = size; }
		constexpr void setSpacing(float spacing) noexcept { _spacing = spacing; }
		void skip(float distance) noexcept;

	private:
		GuiFrame& _frame;
		GuiLayout* const _previous;
		float _scaling = 1;
		seir::Vec2 _offset{ 0, 0 };
		SizeF _size;
		seir::Vec2 _direction{ 1, 1 };
		seir::Vec2 _position{ 0, 0 };
		Axis _axis = Axis::X;
		float _spacing = 0;
		SizeF _defaultSize;
	};
}
