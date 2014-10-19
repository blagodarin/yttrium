#ifndef __GUI_WIDGETS_INPUT_H
#define __GUI_WIDGETS_INPUT_H

#include <yttrium/texture_font.h>

#include "../logic/line_editor.h"
#include "../property.h"
#include "widget.h"

namespace Yttrium
{

class GuiInput: public Widget
{
public:

	GuiInput(Allocator* allocator);

	void dump(GuiPropertyDumper*) const override;
	bool load(GuiPropertyLoader&) override;
	bool process_key(const KeyEvent&) override;
	void render(Renderer&, const RectF&, const Vector2f&, WidgetState) const override;

private:

	void update_rect(const StaticString& text);

private:

	Rect            _position;
	Vector4f        _color;
	TextureProperty _texture;
	TextureFont     _font;
	Texture2DPtr    _font_texture;
	Vector2f        _text_size;
	Vector4f        _text_color;
	LineEditor      _logic;
};

} // namespace Yttrium

#endif // __GUI_WIDGETS_INPUT_H
