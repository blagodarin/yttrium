#include "image.h"

#include <yttrium/log.h>
#include "../property_dumper.h"
#include "../property_loader.h"

// TODO: Find a better class name.

namespace Yttrium
{
	void GuiImage::dump(GuiPropertyDumper& dumper) const
	{
		dumper.dump_rect("position", _position);
		dumper.dump_scaling("scale", _scaling);
		_background.dump(dumper);
	}

	bool GuiImage::load(GuiPropertyLoader& loader)
	{
		if (!loader.load_rect("position", &_position))
		{
			Y_LOG("[Gui.Image] Unable to load");
			return false;
		}

		loader.load_scaling("scale", &_scaling);
		_background.load(loader);

		_rect = RectF(_position);

		return true;
	}

	void GuiImage::render(Renderer& renderer, const RectF& rect, const Vector2&, WidgetState) const
	{
		_background.draw(renderer, rect);
	}
}
