#include "widget.h"

namespace Yttrium
{
	Widget::Widget(GuiPrivate& gui, Flags<Flag> flags)
		: _gui{gui}
		, _flags{flags}
	{
	}
}
