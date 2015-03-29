#ifndef __GUI_WIDGETS_IMAGE_H
#define __GUI_WIDGETS_IMAGE_H

#include "../property.h"
#include "widget.h"

namespace Yttrium
{
	class GuiImage: public Widget
	{
	public:

		GuiImage(Allocator* allocator);

		void dump(GuiPropertyDumper&) const override;
		bool load(GuiPropertyLoader&) override;
		void render(Renderer&, const RectF&, const Vector2f&, WidgetState) const override;

	private:

		Rect               _position;
		BackgroundProperty _background;
	};
}

#endif // __GUI_WIDGETS_IMAGE_H
