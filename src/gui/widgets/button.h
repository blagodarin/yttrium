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
		explicit ButtonWidget(GuiPrivate& gui) : Widget(gui) {}

		bool load(GuiPropertyLoader&) override;
		bool process_key(const KeyEvent&) override;
		void render(Renderer&, const RectF&, WidgetState) const override;

	private:
		struct Style
		{
			BackgroundProperty background;
			Vector4            text_color{1, 1, 1};
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
