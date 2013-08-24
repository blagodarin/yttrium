#ifndef __GUI_WIDGETS_IMAGE_H
#define __GUI_WIDGETS_IMAGE_H

#include "../property.h"
#include "widget.h"

namespace Yttrium
{

namespace Gui
{

class Image
	: public Widget
{
public:

	Image(Allocator *allocator);

public: // Widget

	void dump(PropertyDumper *dumper) const override;

	bool load(PropertyLoader &loader) override;

	void render(Renderer *renderer, const RectF &area, const Vector2f &scale, WidgetState state) const override;

private:

	RectI           _position;
	Vector4f        _color;
	TextureProperty _texture;
};

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_WIDGETS_IMAGE_H
