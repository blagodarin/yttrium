#include "../logging.h"

#include "image.h"

#include <Yttrium/renderer.h>

#include "../property_dumper.h"
#include "../property_loader.h"

namespace Yttrium
{

namespace Gui
{

Image::Image()
	: _color(1, 1, 1)
{
}

void Image::dump(PropertyDumper *dumper) const
{
	dumper->dump_position("pos", _position);
	dumper->dump_size("size", _size);
	dumper->dump_scaling("scale", _scaling);
	dumper->dump_color("color", _color);
	dumper->dump_texture("texture", _texture);
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
	loader.load_texture("texture", &_texture);

	_area = RectF(_position.xy(), _size);

	return true;
}

void Image::render(Renderer *renderer, const RectF &area, const Vector2f &, WidgetState) const
{
	renderer->set_color(_color);
	renderer->set_texture(_texture);
	renderer->draw_rectangle(area.left(), area.top(), area.width(), area.height());
}

} // namespace Gui

} // namespace Yttrium
