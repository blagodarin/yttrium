#include "image.h"

#include <yttrium/static_string.h>
#include "../property_loader.h"

namespace Yttrium
{
	ImageWidget::ImageWidget(GuiPrivate& gui, GuiPropertyLoader& loader)
		: Widget{gui}
	{
		loader.load_rect("position"_s, _rect);
		_background.load(loader);
	}

	void ImageWidget::draw(Renderer& renderer, const RectF& rect, WidgetState) const
	{
		_background.draw(renderer, rect);
	}
}
