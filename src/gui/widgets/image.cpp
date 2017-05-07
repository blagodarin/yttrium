#include "image.h"

#include "../property_loader.h"

namespace Yttrium
{
	ImageWidget::ImageWidget(GuiPrivate& gui, std::string_view name, GuiPropertyLoader& loader)
		: Widget{gui, name}
	{
		loader.load_rect("position", _rect);
		_background.load(loader);
	}

	void ImageWidget::draw(Renderer& renderer, const RectF& rect, WidgetState) const
	{
		_background.draw(renderer, rect);
	}
}
