#include "../logging.h"

#include "input.h"

#include <yttrium/renderer.h>
#include <yttrium/script/context.h>

#include "../property_dumper.h"
#include "../property_loader.h"

namespace Yttrium
{

GuiInput::GuiInput(Allocator* allocator)
	: Widget(allocator, CanHaveFocus)
	, _logic(allocator)
{
}

void GuiInput::dump(GuiPropertyDumper& dumper) const
{
	dumper.dump_rect("position", _position);
	dumper.dump_scaling("scale", _scaling);
	_background.dump(dumper);
	_foreground.dump(dumper);
}

bool GuiInput::load(GuiPropertyLoader& loader)
{
	Y_LOG_TRACE("[Gui.Input] Loading...");

	if (!(loader.load_rect("position", &_position)
		&& _foreground.load(loader)))
	{
		Y_LOG_DEBUG("[Gui.Input] Unable to load");
		return false;
	}

	loader.load_scaling("scale", &_scaling);
	_background.load(loader);

	_rect = RectF(_position);

	return true;
}

bool GuiInput::process_key(const KeyEvent& event)
{
	return event.key == Key::Mouse1 && event.pressed;
}

void GuiInput::render(Renderer& renderer, const RectF& rect, const Vector2f& scale, WidgetState) const
{
	_background.render(renderer, rect);

	const String& text = _logic.text();
	if (!text.is_empty())
	{
		renderer.set_color(_foreground.color);
		renderer.set_texture(_foreground.font_texture);
		if (renderer.set_font(_foreground.font))
		{
			renderer.set_font_size(_foreground.size.x * scale.y, _foreground.size.y);
			renderer.draw_text(rect.center(), text, CenterAlignment);
		}
	}

	if (_is_focused)
	{
		// TODO: Draw cursor.
	}
}

} // namespace Yttrium
