#ifndef __GUI_WIDGETS_BUTTON_H
#define __GUI_WIDGETS_BUTTON_H

#include <yttrium/audio/sound.h>
#include <yttrium/renderer/pointers.h>
#include <yttrium/texture_font.h>

#include "widget.h"

namespace Yttrium
{

namespace Gui
{

class Button
	: public Widget
{
public:

	Button(Allocator *allocator);

public: // Widget

	void dump(PropertyDumper *dumper) const override;

	bool load(PropertyLoader &loader) override;

	void play() const override;

	void render(Renderer *renderer, const RectF &area, const Vector2f &scale, WidgetState state) const override;

private:

	struct Style
	{
		Vector4f     color;
		Texture2DPtr texture;
		Vector4f     text_color;

		Style()
			: color(1, 1, 1)
			, text_color(1, 1, 1)
		{
		}
	};

private:

	Vector3f     _position;
	Vector2f     _size;
	TextureFont  _font;
	Texture2DPtr _font_texture;
	Vector2f     _text_size;
	SoundPtr     _sound;
	WidgetState  _state;
	Style        _styles[WidgetStateCount];
};

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_WIDGETS_BUTTON_H
