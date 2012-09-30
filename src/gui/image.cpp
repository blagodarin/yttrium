#include "logging.h"

#include <Yttrium/renderer.h>

#include "image.h"
#include "property_loader.h"

namespace Yttrium
{

namespace Gui
{

Image::Image()
	: _color(1, 1, 1)
{
}

bool Image::load(const PropertyLoader &loader)
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

void Image::render(Renderer *renderer, const RectF &area, const Vector2f &, WidgetState)
{
	renderer->set_color(_color);
	renderer->set_texture(_texture);
	renderer->draw_rectangle(area.left(), area.top(), area.width(), area.height());
}

} // namespace Gui

} // namespace Yttrium
