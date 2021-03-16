// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/layout.h>

namespace Yt
{
	RectF GuiLayout::add(float width, float height) noexcept
	{
		const auto x = _position.x + width * _direction.x;
		const auto y = _position.y + height * _direction.y;
		const RectF rect{ { _direction.x > 0 ? _position.x : x, _direction.y > 0 ? _position.y : y }, SizeF{ width, height } * _scaling };
		if (_axis == Axis::X)
			_position.x = x + _spacing * _direction.x;
		else
			_position.y = y + _spacing * _direction.y;
		return rect;
	}

	void GuiLayout::fromBottomLeft(Axis axis, float padding) noexcept
	{
		_direction = { _scaling, -_scaling };
		_position = _rect.bottom_left() + padding * _direction;
		_axis = axis;
	}

	void GuiLayout::fromBottomRight(Axis axis, float padding) noexcept
	{
		_direction = { -_scaling, -_scaling };
		_position = _rect.bottom_right() + padding * _direction;
		_axis = axis;
	}
}
