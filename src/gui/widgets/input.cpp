#include "input.h"

#include <yttrium/gui/text_capture.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/script/context.h>
#include "../gui.h"
#include "../property_loader.h"

namespace Yttrium
{
	bool InputWidget::load(GuiPropertyLoader& loader)
	{
		if (!loader.load_rect("position"_s, _rect)
			|| !_foreground.load(loader))
			return false;
		_background.load(loader);
		_on_update = loader.load_actions("on_update"_s);
		_on_enter = loader.load_actions("on_enter"_s);
		std::string initial_text;
		if (loader.load_text("text"_s, initial_text)) // TODO: Use init-statement in C++17.
		{
			_logic.reset(std::move(initial_text));
			_logic.process_key({ Key::End, true, false });
		}
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
				_cursor_mark = std::chrono::steady_clock::now();
				return true;

			default:
				if (_logic.process_key(event))
				{
					_gui.script_context().set("_", StaticString{ _logic.text() });
					_on_update.run(_gui);
					_cursor_mark = std::chrono::steady_clock::now();
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

		if (is_focused() && capture.has_cursor && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _cursor_mark).count() % 1000 < 500)
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
