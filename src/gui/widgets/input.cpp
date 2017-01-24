#include "input.h"

#include <yttrium/gui/text_capture.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/script/context.h>
#include <yttrium/time.h>
#include "../gui.h"
#include "../property_loader.h"

#include <utility>

namespace Yttrium
{
	InputWidget::InputWidget(GuiPrivate& gui)
		: Widget(gui, CanHaveFocus)
		, _foreground(_gui.allocator())
		, _logic(_gui.allocator())
	{
	}

	bool InputWidget::load(GuiPropertyLoader& loader)
	{
		if (!loader.load_rect("position"_s, _rect)
			|| !_foreground.load(loader))
			return false;
		_background.load(loader);
		_on_enter = loader.load_actions("on_enter"_s);
		return true;
	}

	bool InputWidget::process_key(const KeyEvent& event)
	{
		if (event.pressed)
		{
			switch (event.key)
			{
			case Key::Enter:
				_on_enter.run(_gui);
				return true;

			case Key::Mouse1:
				_cursor_mark = millisecond_clock();
				return true;

			default:
				if (_logic.process_key(event))
				{
					_cursor_mark = millisecond_clock();
					return true;
				}
			}
		}
		return false;
	}

	void InputWidget::render(Renderer& renderer, const RectF& rect, WidgetState) const
	{
		_background.draw(renderer, rect);

		TextCapture capture(_logic.cursor(), _logic.selection_offset(), _logic.selection_size());
		_foreground.prepare(_logic.text(), rect, &capture);
		_foreground.draw(renderer);

		if (is_focused() && capture.has_cursor && (millisecond_clock() - _cursor_mark) % 1000 < 500)
		{
			// TODO: Force a cursor symbol to be included in every font.
			PushTexture push_texture(renderer, nullptr);
			renderer.draw_rect(capture.cursor_rect, _foreground.color);
		}

		if (capture.has_selection)
		{
			auto selection_color = _foreground.color;
			selection_color.a *= .25f;
			PushTexture push_texture(renderer, nullptr);
			renderer.draw_rect(capture.selection_rect, selection_color);
		}
	}
}
