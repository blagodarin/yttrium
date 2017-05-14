#ifndef _src_gui_widgets_input_h_
#define _src_gui_widgets_input_h_

#include "../logic/line_editor.h"
#include "../widget.h"

#include <chrono>

namespace Yttrium
{
	class InputWidget : public Widget
	{
	public:
		InputWidget(GuiPrivate&, std::string_view name, std::unique_ptr<WidgetData>&&);

		void draw(Renderer&, const RectF&, WidgetData::Style) const override;
		bool process_key(const KeyEvent&) override;
		bool process_mouse_press(Key, const Vector2&) override;

	private:
		LineEditor _logic;
		std::chrono::steady_clock::time_point _cursor_mark;
	};
}

#endif
