#include "input.h"

#include <yttrium/log.h>
#include <yttrium/renderer.h>
#include <yttrium/renderer_modifiers.h>
#include <yttrium/script/context.h>
#include <yttrium/text_capture.h>
#include "../gui.h"
#include "../property_loader.h"

#include <utility>

namespace Yttrium
{
	InputWidget::InputWidget(const GuiPrivate& gui)
		: Widget(gui, CanHaveFocus)
		, _foreground(_gui.allocator())
		, _logic(_gui.allocator())
	{
	}

	bool InputWidget::load(GuiPropertyLoader& loader)
	{
		if (!(loader.load_rect("position"_s, _rect)
			&& _foreground.load(loader)))
		{
			Log() << "Failed to load 'input'"_s;
			return false;
		}

		_background.load(loader);

		String on_enter(_name.allocator());
		loader.load_text("on_enter"_s, &on_enter);
		_on_enter = ScriptCode(std::move(on_enter), _gui.script_context().allocator());

		return true;
	}

	bool InputWidget::process_key(const KeyEvent& event)
	{
		if (event.pressed > 0)
		{
			switch (event.key)
			{
			case Key::Enter:
				_on_enter.execute(_gui.script_context());
				return true;

			case Key::Mouse1:
				_cursor_mark = Timer::clock();
				return true;

			default:
				if (_logic.process_key(event))
				{
					_cursor_mark = Timer::clock();
					return true;
				}
			}
		}
		return false;
	}

	void InputWidget::render(Renderer& renderer, const RectF& rect, const Vector2&, WidgetState) const
	{
		_background.draw(renderer, rect);

		TextCapture capture(_logic.cursor(), _logic.selection_offset(), _logic.selection_size());
		_foreground.prepare(_logic.text(), rect, &capture);
		_foreground.draw(renderer);

		if (_is_focused && capture.has_cursor && (Timer::clock() - _cursor_mark) % 1000 < 500)
		{
			// TODO: Force a cursor symbol to be included in every font.
			renderer.set_color(_foreground.color);
			PushTexture push_texture(renderer, nullptr);
			renderer.draw_rect(capture.cursor_rect);
		}

		if (capture.has_selection)
		{
			auto selection_color(_foreground.color);
			selection_color.a *= .25f;
			renderer.set_color(selection_color);
			PushTexture push_texture(renderer, nullptr);
			renderer.draw_rect(capture.selection_rect);
		}
	}
}
