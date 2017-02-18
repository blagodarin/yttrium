#include "label.h"

#include <yttrium/renderer/renderer.h>
#include <yttrium/script/context.h>
#include "../gui.h"
#include "../property_loader.h"

namespace Yttrium
{
	bool LabelWidget::load(GuiPropertyLoader& loader)
	{
		return loader.load_rect("position"_s, _rect)
			&& loader.load_translatable("text"_s, _text)
			&& _foreground.load(loader);
	}

	void LabelWidget::render(Renderer& renderer, const RectF& rect, WidgetState) const
	{
		_gui.script_context().substitute(_final_text, _text);
		_foreground.prepare(_final_text, rect);
		_foreground.draw(renderer);
	}
}
