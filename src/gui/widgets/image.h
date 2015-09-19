#ifndef _src_gui_widgets_image_h_
#define _src_gui_widgets_image_h_

#include "../property.h"
#include "widget.h"

namespace Yttrium
{
	class GuiImage : public Widget // TODO: Find a better class name.
	{
	public:

		GuiImage(Allocator* allocator) : Widget(allocator) {}

		void dump(GuiPropertyDumper&) const override;
		bool load(GuiPropertyLoader&) override;
		void render(Renderer&, const RectF&, const Vector2&, WidgetState) const override;

	private:

		Rect               _position;
		BackgroundProperty _background;
	};
}

#endif
