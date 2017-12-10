#ifndef _src_gui_widgets_image_h_
#define _src_gui_widgets_image_h_

#include "../widget.h"

namespace Yttrium
{
	class ImageWidget : public Widget
	{
	public:
		ImageWidget(GuiPrivate&, std::string_view name, std::unique_ptr<WidgetData>&&);

		void draw(RenderContext&, const RectF&, WidgetData::StyleData&) const override;
	};
}

#endif
