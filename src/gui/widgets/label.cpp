#include "../logging.h"

#include "label.h"

#include <yttrium/renderer.h>
#include <yttrium/script/context.h>

#include "../property_dumper.h"
#include "../property_loader.h"

namespace Yttrium
{

Label::Label(Allocator* allocator)
	: Widget(allocator)
	, _alignment(0)
	, _final_text(allocator)
{
}

void Label::dump(GuiPropertyDumper& dumper) const
{
	dumper.dump_position("position", _position);
	dumper.dump_scaling("scale", _scaling);
	_foreground.dump(dumper);
	dumper.dump_alignment("align", _alignment);
	dumper.dump_text("text", _text);
}

bool Label::load(GuiPropertyLoader& loader)
{
	Y_LOG_TRACE("[Gui.Label] Loading...");

	if (!(loader.load_position("position", &_position)
		&& _foreground.load(loader)
		&& loader.load_text("text", &_text)))
	{
		Y_LOG_DEBUG("[Gui.Label] Unable to load");
		return false;
	}

	loader.load_scaling("scale", &_scaling);
	loader.load_alignment("align", &_alignment);

	update_rect(_text);

	return true;
}

void Label::render(Renderer& renderer, const RectF& rect, const Vector2f& scale, WidgetState) const
{
	_foreground.draw(renderer, _final_text, rect.center(), 0, scale.y);
}

void Label::update()
{
	_final_text.clear(); // TODO: Consider moving this into substitute().
	ScriptContext::global().substitute(&_final_text, _text);
	update_rect(_final_text);
}

void Label::update_rect(const StaticString& text)
{
	_rect.set_top_left(_position);

	const Vector2f& text_size = _foreground.font.text_size(text, _foreground.size);

	if ((_alignment & HorizontalAlignmentMask) != RightAlignment)
		_rect.set_left(_rect.left() - text_size.x * (_alignment & LeftAlignment ? 1.0 : 0.5));

	if ((_alignment & VerticalAlignmentMask) != BottomAlignment)
		_rect.set_top(_rect.top() - text_size.y * (_alignment & TopAlignment ? 1.0 : 0.5));

	_rect.set_size(text_size);
}

} // namespace Yttrium
