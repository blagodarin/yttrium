#include "../logging.h"

#include "image.h"

#include <yttrium/renderer.h>

#include "../property_dumper.h"
#include "../property_loader.h"

namespace Yttrium
{

// TODO: Find a better class name.

GuiImage::GuiImage(Allocator* allocator)
	: Widget(allocator)
	, _color(1, 1, 1)
{
}

void GuiImage::dump(GuiPropertyDumper* dumper) const
{
	dumper->dump_rect("position", _position);
	dumper->dump_scaling("scale", _scaling);
	dumper->dump_color("color", _color);
	_texture.dump(dumper);
}

bool GuiImage::load(GuiPropertyLoader& loader)
{
	Y_LOG_TRACE("[Gui.Image] Loading...");

	if (!loader.load_rect("position", &_position))
	{
		Y_LOG_DEBUG("[Gui.Image] Unable to load");
		return false;
	}

	loader.load_scaling("scale", &_scaling);
	loader.load_color("color", &_color);
	_texture.load(loader);

	_rect = RectF(_position);

	return true;
}

void GuiImage::render(Renderer& renderer, const RectF& rect, const Vector2f&, WidgetState) const
{
	renderer.set_color(_color);
	renderer.set_texture(_texture.texture);
	renderer.set_texture_rectangle(RectF(_texture.rect));
	renderer.set_texture_borders(_texture.borders);
	renderer.draw_rectangle(rect.left(), rect.top(), rect.width(), rect.height());
}

} // namespace Yttrium
