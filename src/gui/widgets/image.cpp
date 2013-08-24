#include "../logging.h"

#include "image.h"

#include <yttrium/renderer.h>

#include "../property_dumper.h"
#include "../property_loader.h"

namespace Yttrium
{

namespace Gui
{

Image::Image(Allocator *allocator)
	: Widget(allocator)
	, _color(1, 1, 1)
{
}

void Image::dump(PropertyDumper *dumper) const
{
	dumper->dump_position("pos", _position);
	dumper->dump_size("size", _size);
	dumper->dump_scaling("scale", _scaling);
	dumper->dump_color("color", _color);
	_texture.dump(dumper);
}

bool Image::load(PropertyLoader &loader)
{
	Y_LOG_TRACE("[Gui.Image] Loading...");

	if (!(loader.load_position("pos", &_position)
		&& loader.load_size("size", &_size)))
	{
		Y_LOG_DEBUG("[Gui.Image] Unable to load");
		return false;
	}

	loader.load_scaling("scale", &_scaling);
	loader.load_color("color", &_color);
	_texture.load(loader);

	_area = RectF(_position.xy(), _size);

	return true;
}

void Image::render(Renderer *renderer, const RectF &area, const Vector2f &, WidgetState) const
{
	renderer->set_color(_color);
	renderer->set_texture(_texture.texture);
	renderer->set_texture_rectangle(_texture.rect);
	// TODO: Use texture border.
	renderer->draw_rectangle(area.left(), area.top(), area.width(), area.height());
}

} // namespace Gui

} // namespace Yttrium
