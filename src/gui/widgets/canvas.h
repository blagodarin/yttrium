#ifndef __GUI_WIDGETS_CANVAS_H
#define __GUI_WIDGETS_CANVAS_H

#include "widget.h"

namespace Yttrium
{
	class WindowCallbacks;

	class Canvas: public Widget
	{
	public:

		Canvas(WindowCallbacks& window_callbacks, Allocator* allocator);

		void dump(GuiPropertyDumper&) const override;
		bool load(GuiPropertyLoader&) override;
		void render(Renderer&, const RectF&, const Vector2&, WidgetState) const override;

	private:

		Rect             _position;
		WindowCallbacks& _window_callbacks;
	};
}

#endif // __GUI_WIDGETS_CANVAS_H
