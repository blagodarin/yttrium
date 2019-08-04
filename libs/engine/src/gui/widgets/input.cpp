//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#include "input.h"

#include <yttrium/gui/text_capture.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/pass.h>
#include <yttrium/script/context.h>
#include "../gui.h"

namespace Yt
{
	InputWidget::InputWidget(GuiPrivate& gui, std::unique_ptr<WidgetData>&& data)
		: Widget{ gui, std::move(data), Flag::CanHaveFocus }
	{
		_logic.reset(std::string{ _data->_text });
	}

	void InputWidget::draw(RenderPass& pass, const RectF& rect, WidgetData::StyleData& style_data) const
	{
		style_data._background.draw(pass, rect);

		TextCapture capture{ _logic.cursor(), _logic.selection_offset(), _logic.selection_size() };
		style_data._foreground.prepare(_logic.text(), rect, &capture);
		style_data._foreground.draw(pass);

		if (is_focused() && capture._has_cursor && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _cursor_mark).count() % 1000 < 500)
		{
			PushTexture push_texture{ pass, nullptr };
			pass.draw_rect(capture._cursor_rect, style_data._foreground._color);
		}

		if (capture._has_selection)
		{
			auto selection_color = style_data._foreground._color;
			selection_color._a *= .25f;
			PushTexture push_texture{ pass, nullptr };
			pass.draw_rect(capture._selection_rect, selection_color);
		}
	}

	bool InputWidget::process_key(const KeyEvent& event)
	{
		if (event._pressed)
		{
			switch (event._key)
			{
			case Key::Enter:
				_data->run(_gui, WidgetData::Action::OnEnter);
				return true;

			default:
				if (_logic.process_key(event))
				{
					update_input();
					return true;
				}
			}
		}
		return false;
	}

	bool InputWidget::process_text(std::string_view text)
	{
		_logic.insert(text);
		update_input();
		return true;
	}

	bool InputWidget::process_mouse_press(Key key, const Vector2&)
	{
		if (key != Key::Mouse1)
			return false;
		_cursor_mark = std::chrono::steady_clock::now();
		return true;
	}

	void InputWidget::update_input()
	{
		_gui.script_context().set("_", _logic.text());
		_data->run(_gui, WidgetData::Action::OnUpdate);
		_cursor_mark = std::chrono::steady_clock::now();
	}
}
