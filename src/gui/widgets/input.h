#ifndef _src_gui_widgets_input_h_
#define _src_gui_widgets_input_h_

#include <yttrium/script/code.h>
#include <yttrium/timer.h>
#include "../logic/line_editor.h"
#include "../properties.h"
#include "widget.h"

namespace Yttrium
{
	class InputWidget : public Widget
	{
	public:
		InputWidget(const GuiImpl&);

		bool load(GuiPropertyLoader&) override;
		bool process_key(const KeyEvent&) override;
		void render(Renderer&, const RectF&, const Vector2&, WidgetState) const override;

	private:
		BackgroundProperty _background;
		ForegroundProperty _foreground;
		LineEditor         _logic;
		Clock              _cursor_mark = 0;
		ScriptCode         _on_enter;
	};
}

#endif
