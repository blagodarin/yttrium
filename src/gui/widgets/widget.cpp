#include "widget.h"

#include "../gui.h"

namespace Yttrium
{
	bool Widget::process_key(const KeyEvent&)
	{
		return false;
	}

	Widget::Widget(GuiPrivate& gui, unsigned flags)
		: _gui(gui)
		, _flags(flags)
	{
	}
}
