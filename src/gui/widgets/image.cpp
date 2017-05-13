#include "image.h"

namespace Yttrium
{
	ImageWidget::ImageWidget(GuiPrivate& gui, std::string_view name, std::unique_ptr<WidgetData>&& data)
		: Widget{gui, name, std::move(data)}
	{
	}

	void ImageWidget::draw(Renderer& renderer, const RectF& rect, WidgetData::Style style) const
	{
		_data->style_data(style)._background.draw(renderer, rect);
	}
}
