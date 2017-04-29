#include "canvas.h"

#include <yttrium/exceptions.h>
#include "../gui.h"
#include "../property_loader.h"

namespace Yttrium
{
	CanvasWidget::CanvasWidget(GuiPrivate& gui, GuiPropertyLoader& loader)
		: Widget{gui}
	{
		if (!loader.load_text("id"_s, _id))
			throw GuiDataError{"Bad 'canvas'"_s};
		loader.load_rect("position"_s, _rect);
	}

	void CanvasWidget::draw(Renderer& renderer, const RectF& rect, WidgetState) const
	{
		_gui.draw_canvas(renderer, _id, rect);
	}
}
