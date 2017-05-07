#ifndef _src_gui_widgets_canvas_h_
#define _src_gui_widgets_canvas_h_

#include "widget.h"

namespace Yttrium
{
	class CanvasWidget : public Widget
	{
	public:
		CanvasWidget(GuiPrivate&, std::string_view name, GuiPropertyLoader&);

		void draw(Renderer&, const RectF&, WidgetState) const override;
		void process_mouse_move(const Vector2&) override;
		bool process_mouse_press(Key, const Vector2&) override;
	};
}

#endif
