#ifndef __GUI_WIDGETS_CANVAS_H
#define __GUI_WIDGETS_CANVAS_H

#include <Yttrium/gui/manager.h>
#include <Yttrium/renderer/texture.h>

#include "widget.h"

namespace Yttrium
{

namespace Gui
{

class Canvas: public Widget
{
public:

	Canvas(Manager::Callbacks *manager_callbacks, Allocator *allocator);

public: // Widget

	virtual void dump(PropertyDumper *dumper) const;

	virtual bool load(PropertyLoader &loader);

	virtual void render(Renderer *renderer, const RectF &area, const Vector2f &scale, WidgetState state) const;

private:

	Manager::Callbacks *_manager_callbacks;
	Vector3f            _position;
	Vector2f            _size;
};

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_WIDGETS_CANVAS_H