#include "label.h"

#include <yttrium/script/context.h>
#include "../gui.h"

namespace Yttrium
{
	LabelWidget::LabelWidget(GuiPrivate& gui, std::string_view name, std::unique_ptr<WidgetData>&& data)
		: Widget{gui, name, std::move(data)}
	{
	}

	void LabelWidget::draw(Renderer& renderer, const RectF& rect, WidgetData::Style style) const
	{
		_gui.script_context().substitute(_final_text, _data->_text);
		auto& style_data = _data->style_data(style); // TODO: Move up the call stack?
		style_data._foreground.prepare(_final_text, rect);
		style_data._foreground.draw(renderer);
	}
}
