#include "image.h"

#include <yttrium/log.h>
#include "../property_dumper.h"
#include "../property_loader.h"

namespace Yttrium
{
	void ImageWidget::dump(GuiPropertyDumper& dumper) const
	{
		dumper.dump_rect("position"_s, _position);
		dumper.dump_scaling("scale"_s, _scaling);
		_background.dump(dumper);
	}

	bool ImageWidget::load(GuiPropertyLoader& loader)
	{
		if (!loader.load_rect("position"_s, &_position))
		{
			Log() << "(gui/image) Unable to load"_s;
			return false;
		}

		loader.load_scaling("scale", &_scaling);
		_background.load(loader);

		_rect = RectF(_position);

		return true;
	}

	void ImageWidget::render(Renderer& renderer, const RectF& rect, const Vector2&, WidgetState) const
	{
		_background.draw(renderer, rect);
	}
}
