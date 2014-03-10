#ifndef __GUI_WIDGETS_LABEL_H
#define __GUI_WIDGETS_LABEL_H

#include <yttrium/renderer/pointers.h>
#include <yttrium/texture_font.h>

#include "widget.h"

namespace Yttrium
{

class Label: public Widget
{
public:

	Label(Allocator* allocator);

public: // Widget

	void dump(GuiPropertyDumper* dumper) const override;
	bool load(GuiPropertyLoader& loader) override;
	void render(Renderer* renderer, const RectF& area, const Vector2f& scale, WidgetState state) const override;
	void update() override;

private:

	void update_area(const StaticString& text);

private:

	Vector2f     _position;
	Vector2f     _size;
	Vector4f     _color;
	Texture2DPtr _texture;
	TextureFont  _font;
	Alignment    _alignment;
	String       _final_text;
};

} // namespace Yttrium

#endif // __GUI_WIDGETS_LABEL_H
