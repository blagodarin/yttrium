#include "canvas.h"

#include <yttrium/log.h>
#include <yttrium/window.h>
#include "../gui.h"
#include "../property_dumper.h"
#include "../property_loader.h"

namespace Yttrium
{
	CanvasWidget::CanvasWidget(const GuiImpl& gui)
		: Widget(gui)
		, _window_callbacks(gui.callbacks())
	{
	}

	void CanvasWidget::dump(GuiPropertyDumper& dumper) const
	{
		dumper.dump_rect("position"_s, _position);
		dumper.dump_scaling("scale"_s, _scaling);
	}

	bool CanvasWidget::load(GuiPropertyLoader& loader)
	{
		if (!loader.load_rect("position"_s, &_position))
		{
			Log() << "(gui/canvas) Unable to load"_s;
			return false;
		}

		loader.load_scaling("scale"_s, &_scaling);

		_rect = RectF(_position);

		return true;
	}

	void CanvasWidget::render(Renderer& renderer, const RectF& rect, const Vector2&, WidgetState) const
	{
		_window_callbacks.on_render_canvas(renderer, rect, _name);
	}
}
