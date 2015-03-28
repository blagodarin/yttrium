#ifndef __GUI_WIDGETS_CANVAS_H
#define __GUI_WIDGETS_CANVAS_H

#include <yttrium/gui.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/window.h>

#include "widget.h"

namespace Yttrium
{

class Canvas: public Widget
{
public:

	Canvas(Window::Callbacks& window_callbacks, Allocator* allocator);

	void dump(GuiPropertyDumper&) const override;
	bool load(GuiPropertyLoader&) override;
	void render(Renderer&, const RectF&, const Vector2f&, WidgetState) const override;

private:

	Rect               _position;
	Window::Callbacks& _window_callbacks;
};

} // namespace Yttrium

#endif // __GUI_WIDGETS_CANVAS_H
