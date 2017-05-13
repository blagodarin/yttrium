#include "widget.h"

namespace Yttrium
{
	Widget::Widget(GuiPrivate& gui, std::string_view name, std::unique_ptr<WidgetData>&& data, Flags<Flag> flags)
		: _gui{gui}
		, _name{name}
		, _data{std::move(data)}
		, _flags{flags}
	{
	}
}
