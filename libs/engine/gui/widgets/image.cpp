#include "image.h"

namespace Yttrium
{
	ImageWidget::ImageWidget(GuiPrivate& gui, std::string_view name, std::unique_ptr<WidgetData>&& data)
		: Widget{ gui, name, std::move(data) }
	{
	}

	void ImageWidget::draw(RenderPass& pass, const RectF& rect, WidgetData::StyleData& style_data) const
	{
		style_data._background.draw(pass, rect);
	}
}
