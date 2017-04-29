#ifndef _src_gui_widgets_image_h_
#define _src_gui_widgets_image_h_

#include "../properties.h"
#include "widget.h"

namespace Yttrium
{
	class ImageWidget : public Widget
	{
	public:
		ImageWidget(GuiPrivate&, GuiPropertyLoader&);

		void draw(Renderer& renderer, const RectF& rect, WidgetState) const override;

	private:
		BackgroundProperty _background;
	};
}

#endif
