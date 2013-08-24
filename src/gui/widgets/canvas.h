#ifndef __GUI_WIDGETS_CANVAS_H
#define __GUI_WIDGETS_CANVAS_H

#include <yttrium/gui/manager.h>
#include <yttrium/renderer/texture.h>

#include "widget.h"

namespace Yttrium
{

namespace Gui
{

class Canvas
	: public Widget
{
public:

	Canvas(Manager::Callbacks *manager_callbacks, Allocator *allocator);

public: // Widget

	void dump(PropertyDumper *dumper) const override;

	bool load(PropertyLoader &loader) override;

	void render(Renderer *renderer, const RectF &area, const Vector2f &scale, WidgetState state) const override;

private:

	RectI               _position;
	Manager::Callbacks *_manager_callbacks;
};

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_WIDGETS_CANVAS_H
