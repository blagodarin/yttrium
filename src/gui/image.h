#ifndef __GUI_IMAGE_H
#define __GUI_IMAGE_H

#include <Yttrium/renderer/texture.h>

#include "widget.h"

namespace Yttrium
{

class Renderer;

namespace Gui
{

class Image: public Widget
{
public:

	Image();

public: // Widget

	virtual void dump(PropertyDumper *dumper) const;

	virtual bool load(PropertyLoader &loader);

	virtual void render(Renderer *renderer, const RectF &area, const Vector2f &scale, WidgetState state) const;

private:

	Vector3f  _position;
	Vector2f  _size;
	Vector4f  _color;
	Texture2D _texture;
};

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_IMAGE_H
