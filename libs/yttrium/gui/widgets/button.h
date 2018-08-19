#ifndef _src_gui_widgets_button_h_
#define _src_gui_widgets_button_h_

#include "../widget.h"

namespace Yttrium
{
	class ButtonWidget : public Widget
	{
	public:
		ButtonWidget(GuiPrivate&, std::string_view name, std::unique_ptr<WidgetData>&&);

		void draw(RenderPass&, const RectF&, WidgetData::StyleData&) const override;
		bool process_mouse_press(Key, const Vector2&) override;
		void process_mouse_release() override;
	};
}

#endif
