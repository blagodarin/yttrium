// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/image/color.h>

#include <memory>

namespace Yt
{
	class Font;

	struct GuiButtonStyle
	{
		struct State
		{
			Bgra32 _backgroundColor;
			Bgra32 _textColor;
			constexpr State(Bgra32 backgroundColor, Bgra32 textColor) noexcept
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
			Bgra32 _backgroundColor;
			Bgra32 _textColor;
			constexpr State(Bgra32 backgroundColor, Bgra32 textColor) noexcept
				: _backgroundColor{ backgroundColor }, _textColor{ textColor } {}
		};
		std::shared_ptr<const Font> _font;
		float _fontSize = 0;
		State _normal;
		State _hovered;
		State _active;
		Bgra32 _cursorColor;
		Bgra32 _selectionColor;
		GuiEditStyle() noexcept;
	};

	struct GuiLabelStyle
	{
		std::shared_ptr<const Font> _font;
		float _fontSize = 0;
		Bgra32 _textColor;
		GuiLabelStyle() noexcept;
	};
}
