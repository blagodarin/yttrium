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

public: // Widget

	void dump(GuiPropertyDumper* dumper) const override;

	bool load(GuiPropertyLoader& loader) override;

	void render(Renderer* renderer, const RectF& area, const Vector2f& scale, WidgetState state) const override;

private:

	RectI           _position;
	Vector4f        _color;
	TextureProperty _texture;
};

} // namespace Yttrium

#endif // __GUI_WIDGETS_IMAGE_H
