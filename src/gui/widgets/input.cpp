#include "input.h"

#include <yttrium/log.h>
#include <yttrium/renderer.h>
#include "../property_dumper.h"
#include "../property_loader.h"

#include <utility>

namespace Yttrium
{
	void GuiInput::dump(GuiPropertyDumper& dumper) const
	{
		dumper.dump_rect("position"_s, _position);
		dumper.dump_scaling("scale"_s, _scaling);
		_background.dump(dumper);
		_foreground.dump(dumper);
	}

	bool GuiInput::load(GuiPropertyLoader& loader)
	{
		if (!(loader.load_rect("position"_s, &_position)
			&& _foreground.load(loader)))
		{
			Log() << "[Gui.Input] Unable to load"_s;
			return false;
		}

		loader.load_scaling("scale"_s, &_scaling);
		_background.load(loader);

		_rect = RectF(_position);

		String on_enter(_name.allocator());
		loader.load_text("on_enter"_s, &on_enter);
		_on_enter = ScriptCode(std::move(on_enter));

		return true;
	}

	bool GuiInput::process_key(const KeyEvent& event)
	{
		if (event.pressed > 0)
		{
			switch (event.key)
			{
			case Key::Enter:
				_on_enter.execute();
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

	void GuiInput::render(Renderer& renderer, const RectF& rect, const Vector2& scale, WidgetState) const
	{
		_background.draw(renderer, rect);

		Renderer::TextCapture capture(_logic.cursor(), _logic.selection_offset(), _logic.selection_size());
		_foreground.draw(renderer, _logic.text(), rect.center(), CenterAlignment, scale.y, &capture);

		if (_is_focused && capture.has_cursor && (Timer::clock() - _cursor_mark) % 1000 < 500)
		{
			// TODO: Force a cursor symbol to be included in every font.
			renderer.set_color(_foreground.color);
			PushTexture push_texture(renderer, nullptr);
			renderer.draw_rectangle(capture.cursor_rect);
		}

		if (capture.has_selection)
		{
			auto selection_color(_foreground.color);
			selection_color.a *= .25f;
			renderer.set_color(selection_color);
			PushTexture push_texture(renderer, nullptr);
			renderer.draw_rectangle(capture.selection_rect);
		}
	}
}
