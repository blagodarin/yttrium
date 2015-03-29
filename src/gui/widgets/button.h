#ifndef __GUI_WIDGETS_BUTTON_H
#define __GUI_WIDGETS_BUTTON_H

#include <yttrium/script/code.h>

#include "../property.h"
#include "widget.h"

namespace Yttrium
{
	class Sound;

	class Button: public Widget
	{
	public:

		Button(Allocator* allocator);

		void dump(GuiPropertyDumper&) const override;
		bool load(GuiPropertyLoader&) override;
		bool process_key(const KeyEvent&) override;
		void render(Renderer&, const RectF&, const Vector2f&, WidgetState) const override;

	private:

		struct Style
		{
			BackgroundProperty background;
			Vector4f           text_color;

			Style(): text_color(1, 1, 1) {}
		};

		Rect               _position;
		TextureFont        _font;
		Pointer<Texture2D> _font_texture;
		Vector2f           _text_size;
		Pointer<Sound>     _sound;
		ScriptCode         _action;
		WidgetState        _state;
		Style              _styles[WidgetStateCount];
	};
}

#endif // __GUI_WIDGETS_BUTTON_H
