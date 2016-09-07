#include "canvas.h"

#include <yttrium/window.h>
#include "../gui.h"
#include "../property_loader.h"

namespace Yttrium
{
	CanvasWidget::CanvasWidget(const GuiPrivate& gui)
		: Widget(gui)
	{
	}

	bool CanvasWidget::load(GuiPropertyLoader& loader)
	{
		loader.load_rect("position"_s, _rect);
		return true;
	}

	void CanvasWidget::render(Renderer& renderer, const RectF& rect, const Vector2&, WidgetState) const
	{
		_gui.callbacks().on_render_canvas(renderer, rect, _name);
	}
}
