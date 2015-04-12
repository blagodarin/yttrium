#include "canvas.h"

#include <yttrium/log.h>
#include <yttrium/window.h>
#include "../property_dumper.h"
#include "../property_loader.h"

namespace Yttrium
{
	Canvas::Canvas(WindowCallbacks& window_callbacks, Allocator* allocator)
		: Widget(allocator)
		, _window_callbacks(window_callbacks)
	{
	}

	void Canvas::dump(GuiPropertyDumper& dumper) const
	{
		dumper.dump_rect("position", _position);
		dumper.dump_scaling("scale", _scaling);
	}

	bool Canvas::load(GuiPropertyLoader& loader)
	{
		if (!loader.load_rect("position", &_position))
		{
			Y_LOG("[Gui.Canvas] Unable to load");
			return false;
		}

		loader.load_scaling("scale", &_scaling);

		_rect = RectF(_position);

		return true;
	}

	void Canvas::render(Renderer& renderer, const RectF& rect, const Vector2&, WidgetState) const
	{
		_window_callbacks.on_render_canvas(renderer, rect, _name);
	}
}
