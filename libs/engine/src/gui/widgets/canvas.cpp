//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "canvas.h"

#include "../gui.h"

namespace Yttrium
{
	CanvasWidget::CanvasWidget(GuiPrivate& gui, std::unique_ptr<WidgetData>&& data, std::string_view name)
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
