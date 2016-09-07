#include "widget.h"

#include "../gui.h"

namespace Yttrium
{
	Widget::Widget(const GuiPrivate& gui, unsigned flags)
		: _gui(gui)
		, _text(&_gui.allocator())
		, _name(&_gui.allocator())
		, _flags(flags)
	{
	}

	bool Widget::process_key(const KeyEvent&)
	{
		return false;
	}
}
