#ifndef __GUI_WIDGETS_BUTTON_H
#define __GUI_WIDGETS_BUTTON_H

#include <Yttrium/renderer/texture.h>
#include <Yttrium/texture_font.h>

#include "widget.h"

namespace Yttrium
{

namespace Gui
{

class Button: public Widget
{
public:

	Button(Allocator *allocator);

public: // Widget

	virtual void dump(PropertyDumper *dumper) const;

	virtual bool load(PropertyLoader &loader);

	virtual void render(Renderer *renderer, const RectF &area, const Vector2f &scale, WidgetState state) const;

private:

	struct Style
	{
		Vector4f  color;
		Texture2D texture;
		Vector4f  text_color;

		Style()
			: color(1, 1, 1)
			, text_color(1, 1, 1)
		{
		}
	};

private:

	Vector3f    _position;
	Vector2f    _size;
	TextureFont _font;
	Texture2D   _font_texture;
	Vector2f    _text_size;
	WidgetState _state;
	Style       _styles[WidgetStateCount];
};

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_WIDGETS_BUTTON_H