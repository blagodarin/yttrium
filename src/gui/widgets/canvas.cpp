#include "../logging.h"

#include "canvas.h"

#include <yttrium/renderer.h>

#include "../property_dumper.h"
#include "../property_loader.h"

namespace Yttrium
{

Canvas::Canvas(Gui::Callbacks& gui_callbacks, Allocator* allocator)
	: Widget(allocator)
	, _gui_callbacks(gui_callbacks)
{
}

void Canvas::dump(GuiPropertyDumper* dumper) const
{
	dumper->dump_rect("position", _position);
	dumper->dump_scaling("scale", _scaling);
}

bool Canvas::load(GuiPropertyLoader& loader)
{
	Y_LOG_TRACE("[Gui.Canvas] Loading...");

	if (!loader.load_rect("position", &_position))
	{
		Y_LOG_DEBUG("[Gui.Canvas] Unable to load");
		return false;
	}

	loader.load_scaling("scale", &_scaling);

	_rect = RectF(_position);

	return true;
}

void Canvas::render(Renderer& renderer, const RectF& rect, const Vector2f&, WidgetState) const
{
	_gui_callbacks.on_render_canvas(renderer, _name, rect);
}

} // namespace Yttrium
