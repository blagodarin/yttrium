#ifndef _src_gui_widgets_canvas_h_
#define _src_gui_widgets_canvas_h_

#include "widget.h"

namespace Yttrium
{
	class WindowCallbacks;

	class CanvasWidget : public Widget
	{
	public:
		CanvasWidget(const GuiPrivate&);

		bool load(GuiPropertyLoader&) override;
		void render(Renderer&, const RectF&, WidgetState) const override;

	private:
		String _id;
	};
}

#endif
