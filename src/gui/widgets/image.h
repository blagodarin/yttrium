#ifndef _src_gui_widgets_image_h_
#define _src_gui_widgets_image_h_

#include "../properties.h"
#include "widget.h"

namespace Yttrium
{
	class ImageWidget : public Widget
	{
	public:
		ImageWidget(const GuiPrivate& gui) : Widget(gui) {}

		bool load(GuiPropertyLoader&) override;
		void render(Renderer&, const RectF&, WidgetState) const override;

	private:
		BackgroundProperty _background;
	};
}

#endif
