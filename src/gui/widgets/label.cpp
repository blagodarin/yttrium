#include "label.h"

#include <yttrium/log.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/script/context.h>
#include "../gui.h"
#include "../property_loader.h"

namespace Yttrium
{
	LabelWidget::LabelWidget(const GuiPrivate& gui)
		: Widget(gui)
		, _text(&_gui.allocator())
		, _foreground(_gui.allocator())
		, _final_text(&_gui.allocator())
	{
	}

	bool LabelWidget::load(GuiPropertyLoader& loader)
	{
		if (!loader.load_rect("position"_s, _rect)
			|| !_foreground.load(loader)
			|| !loader.load_translatable("text"_s, &_text))
		{
			Log() << "Failed to load 'label'"_s;
			return false;
		}

		return true;
	}

	void LabelWidget::render(Renderer& renderer, const RectF& rect, WidgetState) const
	{
		_gui.script_context().substitute(_final_text, _text);
		_foreground.prepare(_final_text, rect);
		_foreground.draw(renderer);
	}
}
