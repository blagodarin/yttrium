#ifndef __GUI_WIDGETS_BUTTON_H
#define __GUI_WIDGETS_BUTTON_H

#include <yttrium/audio/sound.h>
#include <yttrium/texture_font.h>

#include "../property.h"
#include "widget.h"

namespace Yttrium
{

class Button: public Widget
{
public:

	Button(Allocator* allocator);

	void dump(GuiPropertyDumper*) const override;
	bool load(GuiPropertyLoader&) override;
	void play() const override;
	void render(Renderer&, const RectF&, const Vector2f&, WidgetState) const override;

private:

	struct Style
	{
		Vector4f        color;
		TextureProperty texture;
		Vector4f        text_color;

		Style()
			: color(1, 1, 1)
			, text_color(1, 1, 1)
		{
		}
	};

private:

	Rect         _position;
	TextureFont  _font;
	Texture2DPtr _font_texture;
	Vector2f     _text_size;
	SoundPtr     _sound;
	WidgetState  _state;
	Style        _styles[WidgetStateCount];
};

} // namespace Yttrium

#endif // __GUI_WIDGETS_BUTTON_H
