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
		_gui.on_canvas_draw(_id, rect, renderer);
	}

	void CanvasWidget::process_mouse_move(const Vector2& cursor)
	{
		_gui.on_canvas_mouse_move(_id, render_rect(), cursor);
	}

	bool CanvasWidget::process_mouse_press(Key key, const Vector2& cursor)
	{
		return _gui.on_canvas_mouse_press(_id, render_rect(), key, cursor);
	}
}
