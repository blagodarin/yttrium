#include "image.h"

#include <yttrium/log.h>
#include "../property_loader.h"

namespace Yttrium
{
	bool ImageWidget::load(GuiPropertyLoader& loader)
	{
		loader.load_rect("position"_s, &_position);
		_background.load(loader);
		_rect = RectF(_position);
		return true;
	}

	void ImageWidget::render(Renderer& renderer, const RectF& rect, const Vector2&, WidgetState) const
	{
		_background.draw(renderer, rect);
	}
}
