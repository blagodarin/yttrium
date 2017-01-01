#ifndef _src_gui_widgets_input_h_
#define _src_gui_widgets_input_h_

#include <yttrium/script/code.h>
#include "../actions.h"
#include "../logic/line_editor.h"
#include "../properties.h"
#include "widget.h"

namespace Yttrium
{
	class InputWidget : public Widget
	{
	public:
		InputWidget(GuiPrivate&);

		bool load(GuiPropertyLoader&) override;
		bool process_key(const KeyEvent&) override;
		void render(Renderer&, const RectF&, WidgetState) const override;

	private:
		BackgroundProperty _background;
		mutable ForegroundProperty _foreground;
		LineEditor _logic;
		int _cursor_mark = 0;
		GuiActions _on_enter;
	};
}

#endif
