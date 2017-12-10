#include "input.h"

#include <yttrium/gui/text_capture.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/script/context.h>
#include "../gui.h"

namespace Yttrium
{
	InputWidget::InputWidget(GuiPrivate& gui, std::string_view name, std::unique_ptr<WidgetData>&& data)
		: Widget{gui, name, std::move(data), Flag::CanHaveFocus}
	{
		_logic.reset(std::string{_data->_text});
	}

	void InputWidget::draw(RenderContext& context, const RectF& rect, WidgetData::StyleData& style_data) const
	{
		style_data._background.draw(context, rect);

		TextCapture capture(_logic.cursor(), _logic.selection_offset(), _logic.selection_size());
		style_data._foreground.prepare(_logic.text(), rect, &capture);
		style_data._foreground.draw(context);

		if (is_focused() && capture._has_cursor && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _cursor_mark).count() % 1000 < 500)
		{
			PushTexture push_texture{context, nullptr};
			context.draw_rect(capture._cursor_rect, style_data._foreground.color);
		}

		if (capture._has_selection)
		{
			auto selection_color = style_data._foreground.color;
			selection_color._a *= .25f;
			PushTexture push_texture{context, nullptr};
			context.draw_rect(capture._selection_rect, selection_color);
		}
	}

	bool InputWidget::process_key(const KeyEvent& event)
	{
		if (event.pressed)
		{
			switch (event.key)
			{
			case Key::Enter:
				_data->run(_gui, WidgetData::Action::OnEnter);
				return true;

			default:
				if (_logic.process_key(event))
				{
					_gui.script_context().set("_", _logic.text());
					_data->run(_gui, WidgetData::Action::OnUpdate);
					_cursor_mark = std::chrono::steady_clock::now();
					return true;
				}
			}
		}
		return false;
	}

	bool InputWidget::process_mouse_press(Key key, const Vector2&)
	{
		if (key != Key::Mouse1)
			return false;
		_cursor_mark = std::chrono::steady_clock::now();
		return true;
	}
}
