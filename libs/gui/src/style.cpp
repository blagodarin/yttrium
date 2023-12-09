// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/style.h>

namespace Yt
{
	GuiButtonStyle::GuiButtonStyle() noexcept
		: _fontSize{ .75f }
		, _normal{ seir::Rgba32::grayscale(0x88, 0xcc), seir::Rgba32::black() }
		, _hovered{ seir::Rgba32::grayscale(0xdd, 0xdd), seir::Rgba32::grayscale(0x11) }
		, _pressed{ seir::Rgba32::white(0xee), seir::Rgba32::grayscale(0x22) }
	{
	}

	GuiEditStyle::GuiEditStyle() noexcept
		: _fontSize{ .75f }
		, _normal{ seir::Rgba32::grayscale(0x00, 0xcc), seir::Rgba32::grayscale(0xdd) }
		, _hovered{ seir::Rgba32::grayscale(0x11, 0xdd), seir::Rgba32::grayscale(0xee) }
		, _active{ seir::Rgba32::grayscale(0x22, 0xee), seir::Rgba32::white() }
		, _cursorColor{ seir::Rgba32::white() }
		, _selectionColor{ seir::Rgba32::grayscale(0x55) }
	{
	}

	GuiLabelStyle::GuiLabelStyle() noexcept
		: _fontSize{ .75f }
		, _textColor{ seir::Rgba32::white() }
	{
	}
}
