// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/layout.h>

namespace Yt
{
	GuiLayout::GuiLayout(const RectF& rect) noexcept
		: _rect{ rect }
	{
	}

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

	void GuiLayout::fromPoint(const Vector2& point, const Vector2& direction, Axis axis, float padding) noexcept
	{
		_direction = direction * _scaling;
		_position = point + padding * _direction;
		_axis = axis;
	}
}
