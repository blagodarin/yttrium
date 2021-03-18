// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/style.h>

namespace Yt
{
	GuiButtonStyle::GuiButtonStyle() noexcept
		: _fontSize{ .75f }
		, _normal{ Bgra32::grayscale(0x88, 0xcc), Bgra32::black() }
		, _hovered{ Bgra32::grayscale(0xdd, 0xdd), Bgra32::grayscale(0x11) }
		, _pressed{ Bgra32::white(0xee), Bgra32::grayscale(0x22) }
	{
	}

	GuiEditStyle::GuiEditStyle() noexcept
		: _fontSize{ .75f }
		, _normal{ Bgra32::grayscale(0x00, 0xcc), Bgra32::grayscale(0xdd) }
		, _hovered{ Bgra32::grayscale(0x11, 0xdd), Bgra32::grayscale(0xee) }
		, _active{ Bgra32::grayscale(0x22, 0xee), Bgra32::white() }
		, _cursorColor{ Bgra32::white() }
	{
	}

	GuiLabelStyle::GuiLabelStyle() noexcept
		: _fontSize{ .75f }
		, _textColor{ Bgra32::white() }
	{
	}
}
