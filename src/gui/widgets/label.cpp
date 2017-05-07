#include "label.h"

#include <yttrium/exceptions.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/script/context.h>
#include "../gui.h"
#include "../property_loader.h"

namespace Yttrium
{
	LabelWidget::LabelWidget(GuiPrivate& gui, std::string_view name, GuiPropertyLoader& loader)
		: Widget{gui, name}
	{
		if (!(loader.load_rect("position", _rect)
			&& loader.load_translatable("text", _text)
			&& _foreground.load(loader)))
			throw GuiDataError{"Bad 'label'"};
	}

	void LabelWidget::draw(Renderer& renderer, const RectF& rect, WidgetState) const
	{
		_gui.script_context().substitute(_final_text, _text);
		_foreground.prepare(_final_text, rect);
		_foreground.draw(renderer);
	}
}
