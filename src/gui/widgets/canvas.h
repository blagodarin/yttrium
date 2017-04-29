#ifndef _src_gui_widgets_canvas_h_
#define _src_gui_widgets_canvas_h_

#include "widget.h"

#include <string>

namespace Yttrium
{
	class CanvasWidget : public Widget
	{
	public:
		CanvasWidget(GuiPrivate&, GuiPropertyLoader&);

		void draw(Renderer&, const RectF&, WidgetState) const override;

	private:
		std::string _id;
	};
}

#endif
