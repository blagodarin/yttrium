// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "canvas.h"

#include "../gui.h"

namespace Yt
{
	CanvasWidget::CanvasWidget(IonGuiPrivate& gui, std::unique_ptr<WidgetData>&& data, std::string_view name)
		: Widget{ gui, std::move(data) }
		, _name{ name }
	{
	}

	void CanvasWidget::draw(RenderPass& pass, const RectF& rect, WidgetData::StyleData&) const
	{
		_gui.on_canvas_draw(pass, _name, rect);
	}

	void CanvasWidget::process_mouse_move(const Vector2& cursor)
	{
		_gui.on_canvas_mouse_move(_name, render_rect(), cursor);
	}

	bool CanvasWidget::process_mouse_press(Key key, const Vector2& cursor)
	{
		return _gui.on_canvas_mouse_press(_name, render_rect(), key, cursor);
	}
}
