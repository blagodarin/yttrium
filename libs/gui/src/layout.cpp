// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/layout.h>

namespace Yt
{
	RectF GuiLayout::add(const SizeF& size) noexcept
	{
		const auto rectSize = size == SizeF{} ? _defaultSize : size;
		const auto x1 = _position.x + rectSize._width * (_direction.x - 1) / 2;
		const auto x2 = _position.x + rectSize._width * (_direction.x + 1) / 2;
		const auto y1 = _position.y + rectSize._height * (_direction.y - 1) / 2;
		const auto y2 = _position.y + rectSize._height * (_direction.y + 1) / 2;
		if (_axis == Axis::X)
			_position.x = _direction.x > 0 ? x2 + _spacing : x1 - _spacing;
		else
			_position.y = _direction.y > 0 ? y2 + _spacing : y1 - _spacing;
		return RectF{ { x1, y1 }, Vector2{ x2, y2 } } * _scaling + _offset;
	}

	void GuiLayout::fromPoint(const Vector2& point, const Vector2& direction, Axis axis, float padding) noexcept
	{
		_direction = direction;
		_position = point + padding * _direction;
		_axis = axis;
	}

	void GuiLayout::mapToCenter(const SizeF& size) noexcept
	{
		const auto widthRatio = _rect.width() / size._width;
		const auto heightRatio = _rect.height() / size._height;
		if (widthRatio > heightRatio)
		{
			_scaling = heightRatio;
			_offset = { (_rect.width() - size._width * _scaling) / 2, 0 };
		}
		else
		{
			_scaling = widthRatio;
			_offset = { 0, (_rect.height() - size._height * _scaling) / 2 };
		}
		_size = size;
	}

	void GuiLayout::scaleForHeight(float height) noexcept
	{
		_scaling = _rect.height() / height;
		_offset = { 0, 0 };
		_size = { _rect.width() / _scaling, height };
	}

	void GuiLayout::scaleForWidth(float width) noexcept
	{
		_scaling = _rect.width() / width;
		_offset = { 0, 0 };
		_size = { width, _rect.height() / _scaling };
	}

	void GuiLayout::skip(float distance) noexcept
	{
		if (_axis == Axis::X)
			_position.x += _direction.x * distance;
		else
			_position.y += _direction.y * distance;
	}
}
