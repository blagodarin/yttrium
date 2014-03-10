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

	Canvas(Gui::Callbacks* gui_callbacks, Allocator* allocator);

public: // Widget

	void dump(GuiPropertyDumper* dumper) const override;
	bool load(GuiPropertyLoader& loader) override;
	void render(Renderer* renderer, const RectF& area, const Vector2f& scale, WidgetState state) const override;

private:

	RectI           _position;
	Gui::Callbacks* _gui_callbacks;
};

} // namespace Yttrium

#endif // __GUI_WIDGETS_CANVAS_H
