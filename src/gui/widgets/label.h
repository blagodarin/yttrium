#ifndef __GUI_WIDGETS_LABEL_H
#define __GUI_WIDGETS_LABEL_H

#include <Yttrium/renderer/texture.h>
#include <Yttrium/texture_font.h>

#include "widget.h"

namespace Yttrium
{

namespace Gui
{

class Label: public Widget
{
public:

	Label(Allocator *allocator);

public: // Widget

	virtual void dump(PropertyDumper *dumper) const;

	virtual bool load(PropertyLoader &loader);

	virtual void render(Renderer *renderer, const RectF &area, const Vector2f &scale, WidgetState state) const;

	virtual void update();

private:

	void update_area(const StaticString &text);

private:

	Vector3f    _position;
	Vector2f    _size;
	Vector4f    _color;
	Texture2D   _texture;
	TextureFont _font;
	Alignment   _alignment;

	String      _final_text;
};

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_WIDGETS_LABEL_H
