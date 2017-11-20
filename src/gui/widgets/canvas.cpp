#include "canvas.h"

#include "../gui.h"

namespace Yttrium
{
	CanvasWidget::CanvasWidget(GuiPrivate& gui, std::string_view name, std::unique_ptr<WidgetData>&& data)
		: Widget{gui, name, std::move(data)}
	{
	}

	void CanvasWidget::draw(Renderer& renderer, const RectF& rect, WidgetData::StyleData&) const
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
