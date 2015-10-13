#ifndef _src_gui_widgets_button_h_
#define _src_gui_widgets_button_h_

#include <yttrium/script/code.h>
#include "../property.h"
#include "widget.h"

namespace Yttrium
{
	class Sound;

	class ButtonWidget : public Widget
	{
	public:

		ButtonWidget(const GuiImpl& gui) : Widget(gui) {}

		void dump(GuiPropertyDumper&) const override;
		bool load(GuiPropertyLoader&) override;
		bool process_key(const KeyEvent&) override;
		void render(Renderer&, const RectF&, const Vector2&, WidgetState) const override;

	private:

		struct Style
		{
			BackgroundProperty background;
			Vector4            text_color;

			Style(): text_color(1, 1, 1) {}
		};

		Rect                 _position;
		TextureFont          _font;
		SharedPtr<Texture2D> _font_texture;
		Vector2              _text_size;
		SharedPtr<Sound>     _sound;
		WidgetState          _state = WidgetState::NotSet;
		Style                _styles[WidgetStateCount];
		ScriptCode           _on_click;
	};
}

#endif
