#include "widget.h"

namespace Yttrium
{
	void Widget::draw(RenderPass& pass, const RectF& rect, WidgetData::Style style) const
	{
		if (_data->_fixed_style)
			style = *_data->_fixed_style;
		return draw(pass, rect, _data->style_data(style));
	}

	Widget::Widget(GuiPrivate& gui, std::string_view name, std::unique_ptr<WidgetData>&& data, Flags<Flag> flags)
		: _gui{gui}
		, _name{name}
		, _data{std::move(data)}
		, _flags{flags}
	{
	}
}
