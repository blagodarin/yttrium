#include "../logging.h"

#include "label.h"

#include <Yttrium/renderer.h>
#include <Yttrium/script/manager.h>

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
	dumper->dump_position("pos", _position);
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

	if (!(loader.load_position("pos", &_position)
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

	// TODO: Merge the following with update().

	_area.set_top_left(_position.xy());

	Vector2f size = _font.text_size(_text, _size);

	if ((_alignment & HorizontalAlignmentMask) != RightAlignment)
	{
		_area.set_left(_area.left() - size.x * (_alignment & LeftAlignment ? 1.0 : 0.5));
	}

	if ((_alignment & VerticalAlignmentMask) != BottomAlignment)
	{
		_area.set_top(_area.top() - size.y * (_alignment & TopAlignment ? 1.0 : 0.5));
	}

	Vector2f area;

	_area.set_size(loader.load_size("area", &area) ? area : size);

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
		renderer->draw_text(area.center(), _final_text, _alignment);
	}
}

void Label::update()
{
	_final_text.clear(); // NOTE: Maybe move this into substitute()?
	ScriptManager::instance()->root_context().substitute(&_final_text, _text);

	_area.set_top_left(_position.xy());

	Vector2f size = _font.text_size(_final_text, _size);

	if ((_alignment & HorizontalAlignmentMask) != RightAlignment)
	{
		_area.set_left(_area.left() - size.x * (_alignment & LeftAlignment ? 1.0 : 0.5));
	}

	if ((_alignment & VerticalAlignmentMask) != BottomAlignment)
	{
		_area.set_top(_area.top() - size.y * (_alignment & TopAlignment ? 1.0 : 0.5));
	}

	_area.set_size(size);
}

} // namespace Gui

} // namespace Yttrium
