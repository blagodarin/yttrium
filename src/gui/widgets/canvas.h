#ifndef __GUI_WIDGETS_CANVAS_H
#define __GUI_WIDGETS_CANVAS_H

#include <yttrium/gui.h>
#include <yttrium/renderer/texture.h>

#include "widget.h"

namespace Yttrium
{

class Canvas: public Widget
{
public:

	Canvas(Gui::Callbacks& gui_callbacks, Allocator* allocator);

	void dump(GuiPropertyDumper&) const override;
	bool load(GuiPropertyLoader&) override;
	void render(Renderer&, const RectF&, const Vector2f&, WidgetState) const override;

private:

	Rect            _position;
	Gui::Callbacks& _gui_callbacks;
};

} // namespace Yttrium

#endif // __GUI_WIDGETS_CANVAS_H
