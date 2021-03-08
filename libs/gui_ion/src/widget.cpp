// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "widget.h"

namespace Yt
{
	Vector2 Widget::clamp(const Vector2& point) const noexcept
	{
		return {
			std::clamp(point.x, render_rect().left(), render_rect().right() - 1),
			std::clamp(point.y, render_rect().top(), render_rect().bottom() - 1),
		};
	}

	void Widget::draw(RenderPass& pass, const RectF& rect, WidgetData::Style style) const
	{
		if (_data->_fixed_style)
			style = *_data->_fixed_style;
		return draw(pass, rect, _data->style_data(style));
	}

	Widget::Widget(IonGuiPrivate& gui, std::unique_ptr<WidgetData>&& data, Flags<Flag> flags)
		: _gui{ gui }
		, _data{ std::move(data) }
		, _flags{ flags }
	{
	}

	Widget::~Widget() noexcept = default;
}
