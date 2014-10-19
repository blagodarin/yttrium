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
	, _text_color(1, 1, 1)
	, _logic(allocator)
{
}

void GuiInput::dump(GuiPropertyDumper* dumper) const
{
	dumper->dump_rect("position", _position);
	dumper->dump_scaling("scale", _scaling);
	dumper->dump_size("text_size", _text_size);
	dumper->dump_color("color", _color);
	dumper->dump_color("text_color", _text_color);

	// TODO: Dump font.

	_texture.dump(dumper);
}

bool GuiInput::load(GuiPropertyLoader& loader)
{
	Y_LOG_TRACE("[Gui.Input] Loading...");

	if (!(loader.load_rect("position", &_position)
		&& loader.load_font("font", &_font, &_font_texture)
		&& loader.load_size("text_size", &_text_size)))
	{
		Y_LOG_DEBUG("[Gui.Input] Unable to load");
		return false;
	}

	loader.load_scaling("scale", &_scaling);
	loader.load_color("color", &_color);
	_texture.load(loader);
	loader.load_color("text_color", &_text_color);

	_rect = RectF(_position);

	return true;
}

bool GuiInput::process_key(const KeyEvent& event)
{
	return event.key == Key::Mouse1 && event.pressed;
}

void GuiInput::render(Renderer& renderer, const RectF& rect, const Vector2f& scale, WidgetState) const
{
	renderer.set_color(_color);
	renderer.set_texture(_texture.texture);
	renderer.set_texture_rectangle(RectF(_texture.rect));
	renderer.set_texture_borders(_texture.borders);
	renderer.draw_rectangle(rect.left(), rect.top(), rect.width(), rect.height());

	const String& text = _logic.text();
	if (!text.is_empty())
	{
		renderer.set_color(_text_color);
		renderer.set_texture(_font_texture);
		if (renderer.set_font(_font))
		{
			renderer.set_font_size(_text_size.x * scale.y, _text_size.y);
			renderer.draw_text(rect.center(), text, CenterAlignment);
		}
	}

	if (_is_focused)
	{
		// TODO: Draw cursor.
	}
}

} // namespace Yttrium
