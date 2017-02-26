#ifndef _src_gui_widgets_input_h_
#define _src_gui_widgets_input_h_

#include "../actions.h"
#include "../logic/line_editor.h"
#include "../properties.h"
#include "widget.h"

#include <chrono>

namespace Yttrium
{
	class InputWidget : public Widget
	{
	public:
		InputWidget(GuiPrivate& gui) : Widget(gui, Flag::CanHaveFocus) {}

		bool load(GuiPropertyLoader&) override;
		bool process_key(const KeyEvent&) override;
		void render(Renderer&, const RectF&, WidgetState) const override;

	private:
		BackgroundProperty _background;
		mutable ForegroundProperty _foreground;
		LineEditor _logic;
		std::chrono::steady_clock::time_point _cursor_mark;
		GuiActions _on_update;
		GuiActions _on_enter;
	};
}

#endif
