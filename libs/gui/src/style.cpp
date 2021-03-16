// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/style.h>

namespace Yt
{
	GuiButtonStyle::GuiButtonStyle() noexcept
		: _fontSize{ .75f }
		, _normal{ Bgra32::grayscale(0x88, 224), Bgra32::black() }
		, _hovered{ Bgra32::grayscale(0xdd, 224), Bgra32::grayscale(0x11) }
		, _pressed{ Bgra32::white(224), Bgra32::grayscale(0x22) }
	{
	}

	GuiLabelStyle::GuiLabelStyle() noexcept
		: _fontSize{ .75f }
		, _textColor{ Bgra32::white() }
	{
	}
}
