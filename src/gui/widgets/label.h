#ifndef _src_gui_widgets_label_h_
#define _src_gui_widgets_label_h_

#include "widget.h"

namespace Yttrium
{
	class LabelWidget : public Widget
	{
	public:
		LabelWidget(GuiPrivate&, std::string_view name, std::unique_ptr<WidgetData>&&);

		void draw(Renderer&, const RectF&, WidgetData::Style) const override;

	private:
		mutable std::string _final_text;
	};
}

#endif
