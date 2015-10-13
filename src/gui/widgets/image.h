#ifndef _src_gui_widgets_image_h_
#define _src_gui_widgets_image_h_

#include "../property.h"
#include "widget.h"

namespace Yttrium
{
	class ImageWidget : public Widget
	{
	public:

		ImageWidget(const GuiImpl& gui) : Widget(gui) {}

		void dump(GuiPropertyDumper&) const override;
		bool load(GuiPropertyLoader&) override;
		void render(Renderer&, const RectF&, const Vector2&, WidgetState) const override;

	private:

		Rect _position;
		BackgroundProperty _background;
	};
}

#endif
