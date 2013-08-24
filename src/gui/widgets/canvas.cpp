#include "../logging.h"

#include "canvas.h"

#include <yttrium/renderer.h>

#include "../property_dumper.h"
#include "../property_loader.h"

namespace Yttrium
{

namespace Gui
{

Canvas::Canvas(Manager::Callbacks *manager_callbacks, Allocator *allocator)
	: Widget(allocator)
	, _manager_callbacks(manager_callbacks)
{
}

void Canvas::dump(PropertyDumper *dumper) const
{
	dumper->dump_rect("position", _position);
	dumper->dump_scaling("scale", _scaling);
}

bool Canvas::load(PropertyLoader &loader)
{
	Y_LOG_TRACE("[Gui.Canvas] Loading...");

	if (!loader.load_rect("position", &_position))
	{
		Y_LOG_DEBUG("[Gui.Canvas] Unable to load");
		return false;
	}

	loader.load_scaling("scale", &_scaling);

	_area = _position;

	return true;
}

void Canvas::render(Renderer *renderer, const RectF &area, const Vector2f &, WidgetState) const
{
	if (_manager_callbacks)
		_manager_callbacks->on_render_canvas(renderer, _name, area);
}

} // namespace Gui

} // namespace Yttrium
