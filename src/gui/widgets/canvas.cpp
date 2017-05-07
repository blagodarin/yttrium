#include "canvas.h"

#include "../gui.h"
#include "../property_loader.h"

namespace Yttrium
{
	CanvasWidget::CanvasWidget(GuiPrivate& gui, std::string_view name, GuiPropertyLoader& loader)
		: Widget{gui, name}
	{
		loader.load_rect("position", _rect);
	}

	void CanvasWidget::draw(Renderer& renderer, const RectF& rect, WidgetState) const
	{
		_gui.on_canvas_draw(_name, rect, renderer);
	}

	void CanvasWidget::process_mouse_move(const Vector2& cursor)
	{
		_gui.on_canvas_mouse_move(_name, render_rect(), cursor);
	}

	bool CanvasWidget::process_mouse_press(Key key, const Vector2& cursor)
	{
		return _gui.on_canvas_mouse_press(_name, render_rect(), key, cursor);
	}
}
