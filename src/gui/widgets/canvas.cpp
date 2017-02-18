#include "canvas.h"

#include "../gui.h"
#include "../property_loader.h"

namespace Yttrium
{
	bool CanvasWidget::load(GuiPropertyLoader& loader)
	{
		if (!loader.load_text("id"_s, _id))
			return false;
		loader.load_rect("position"_s, _rect);
		return true;
	}

	void CanvasWidget::render(Renderer& renderer, const RectF& rect, WidgetState) const
	{
		_gui.draw_canvas(renderer, _id, rect);
	}
}
