// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/layout.h>

#include <yttrium/gui/gui.h>
#include "context_data.h"

namespace Yt
{
	GuiLayout::GuiLayout() noexcept
		: _frame{ nullptr }
		, _previous{ nullptr }
	{
	}

	GuiLayout::GuiLayout(GuiFrame& frame) noexcept
		: _frame{ &frame }
		, _previous{ std::exchange(frame._context._layout, this) }
		, _size{ frame._context._window.size() }
	{
	}

	GuiLayout::GuiLayout(GuiFrame& frame, const Center& mapping) noexcept
		: _frame{ &frame }
		, _previous{ std::exchange(_frame->_context._layout, this) }
	{
		const SizeF viewport{ _frame->_context._window.size() };
		const auto widthRatio = viewport._width / mapping._width;
		const auto heightRatio = viewport._height / mapping._height;
		if (widthRatio > heightRatio)
		{
			_scaling = heightRatio;
			_offset = { (viewport._width - mapping._width * _scaling) / 2, 0 };
		}
		else
		{
			_scaling = widthRatio;
			_offset = { 0, (viewport._height - mapping._height * _scaling) / 2 };
		}
		_size = { mapping ._width, mapping._height};
	}

	GuiLayout::GuiLayout(GuiFrame& frame, const Height& mapping) noexcept
		: _frame{ &frame }
		, _previous{ std::exchange(_frame->_context._layout, this) }
	{
		const SizeF viewport{ _frame->_context._window.size() };
		_scaling = viewport._height / mapping._height;
		_offset = { 0, 0 };
		_size = { viewport._width / _scaling, mapping._height };
	}

	GuiLayout::GuiLayout(GuiFrame& frame, const Width& mapping) noexcept
		: _frame{ &frame }
		, _previous{ std::exchange(_frame->_context._layout, this) }
	{
		const SizeF viewport{ _frame->_context._window.size() };
		_scaling = viewport._width / mapping._width;
		_offset = { 0, 0 };
		_size = { mapping._width, viewport._height / _scaling };
	}

	GuiLayout::~GuiLayout() noexcept
	{
		if (_frame)
			_frame->_context._layout = _previous;
	}

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

	void GuiLayout::skip(float distance) noexcept
	{
		if (_axis == Axis::X)
			_position.x += _direction.x * distance;
		else
			_position.y += _direction.y * distance;
	}
}
