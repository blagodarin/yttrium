#include "widget.h"

namespace Yttrium
{
	Widget::Widget(GuiPrivate& gui, std::string_view name, Flags<Flag> flags)
		: _gui{gui}
		, _name{name}
		, _flags{flags}
	{
	}
}
