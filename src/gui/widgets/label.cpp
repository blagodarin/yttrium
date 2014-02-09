#include "../logging.h"

#include "label.h"

#include <yttrium/renderer.h>
#include <yttrium/script/context.h>

#include "../property_dumper.h"
#include "../property_loader.h"

namespace Yttrium
{

namespace Gui
{

Label::Label(Allocator *allocator)
	: Widget(allocator)
	, _color(1, 1, 1)
	, _alignment(0)
	, _final_text(allocator)
{
}

void Label::dump(PropertyDumper *dumper) const
{
	dumper->dump_position("position", _position);
	dumper->dump_size("size", _size);
	dumper->dump_scaling("scale", _scaling);
	dumper->dump_color("color", _color);

	// TODO: Dump font.

	dumper->dump_alignment("align", _alignment);
	dumper->dump_text("text", _text);
}

bool Label::load(PropertyLoader &loader)
{
	Y_LOG_TRACE("[Gui.Label] Loading...");

	if (!(loader.load_position("position", &_position)
		&& loader.load_size("size", &_size)
		&& loader.load_font("font", &_font, &_texture)
		&& loader.load_text("text", &_text)))
	{
		Y_LOG_DEBUG("[Gui.Label] Unable to load");
		return false;
	}

	loader.load_scaling("scale", &_scaling);
	loader.load_color("color", &_color);
	loader.load_alignment("align", &_alignment);

	update_area(_text);

	return true;
}

void Label::render(Renderer *renderer, const RectF &area, const Vector2f &scale, WidgetState) const
{
	if (_text.is_empty())
	{
		return;
	}

	renderer->set_color(_color);
	renderer->set_texture(_texture);

	if (renderer->set_font(_font))
	{
		renderer->set_font_size(_size.x * scale.y, _size.y);
		renderer->draw_text(area.center(), _final_text, 0);
	}
}

void Label::update()
{
	_final_text.clear(); // TODO: Consider moving this into substitute().
	ScriptContext::global().substitute(&_final_text, _text);
	update_area(_final_text);
}

void Label::update_area(const StaticString &text)
{
	_area.set_top_left(_position);

	Vector2f text_size = _font.text_size(text, _size);

	if ((_alignment & HorizontalAlignmentMask) != RightAlignment)
	{
		_area.set_left(_area.left() - text_size.x * (_alignment & LeftAlignment ? 1.0 : 0.5));
	}

	if ((_alignment & VerticalAlignmentMask) != BottomAlignment)
	{
		_area.set_top(_area.top() - text_size.y * (_alignment & TopAlignment ? 1.0 : 0.5));
	}

	_area.set_size(text_size);
}

} // namespace Gui

} // namespace Yttrium
