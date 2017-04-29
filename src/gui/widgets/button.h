#ifndef _src_gui_widgets_button_h_
#define _src_gui_widgets_button_h_

#include "../actions.h"
#include "../properties.h"
#include "widget.h"

namespace Yttrium
{
	class Sound;

	class ButtonWidget : public Widget
	{
	public:
		ButtonWidget(GuiPrivate&, GuiPropertyLoader&);

		void draw(Renderer&, const RectF&, WidgetState) const override;
		bool process_mouse_press(Key, const Vector2&) override;
		void process_mouse_release() override;

	private:
		struct Style
		{
			BackgroundProperty background;
			Color4f            text_color{1, 1, 1};
		};

		std::string _text;
		mutable ForegroundProperty _foreground;
		std::shared_ptr<const Sound> _sound;
		WidgetState _state = WidgetState::NotSet;
		Style _styles[WidgetStateCount];
		GuiActions _on_click;
	};
}

#endif
