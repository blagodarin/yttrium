#ifndef _src_gui_widgets_canvas_h_
#define _src_gui_widgets_canvas_h_

#include "widget.h"

namespace Yttrium
{
	class WindowCallbacks;

	class CanvasWidget : public Widget
	{
	public:

		CanvasWidget(const GuiImpl&);

		void dump(GuiPropertyDumper&) const override;
		bool load(GuiPropertyLoader&) override;
		void render(Renderer&, const RectF&, const Vector2&, WidgetState) const override;

	private:

		Rect _position;
		WindowCallbacks& _window_callbacks;
	};
}

#endif
