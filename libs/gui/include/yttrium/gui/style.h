// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <seir_graphics/color.hpp>

#include <memory>

namespace Yt
{
	class Font;

	struct GuiButtonStyle
	{
		struct State
		{
			seir::Rgba32 _backgroundColor;
			seir::Rgba32 _textColor;
			constexpr State(seir::Rgba32 backgroundColor, seir::Rgba32 textColor) noexcept
				: _backgroundColor{ backgroundColor }, _textColor{ textColor } {}
		};
		std::shared_ptr<const Font> _font;
		float _fontSize = 0;
		State _normal;
		State _hovered;
		State _pressed;
		GuiButtonStyle() noexcept;
	};

	struct GuiEditStyle
	{
		struct State
		{
			seir::Rgba32 _backgroundColor;
			seir::Rgba32 _textColor;
			constexpr State(seir::Rgba32 backgroundColor, seir::Rgba32 textColor) noexcept
				: _backgroundColor{ backgroundColor }, _textColor{ textColor } {}
		};
		std::shared_ptr<const Font> _font;
		float _fontSize = 0;
		State _normal;
		State _hovered;
		State _active;
		seir::Rgba32 _cursorColor;
		seir::Rgba32 _selectionColor;
		GuiEditStyle() noexcept;
	};

	struct GuiLabelStyle
	{
		std::shared_ptr<const Font> _font;
		float _fontSize = 0;
		seir::Rgba32 _textColor;
		GuiLabelStyle() noexcept;
	};
}
