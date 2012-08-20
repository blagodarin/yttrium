#include "screen.h"

namespace Yttrium
{

Screen::Screen(const Screen &screen)
	: _private(Private::copy(screen._private))
{
}

void Screen::close()
{
	Private::release(&_private);
}

Screen &Screen::operator =(const Screen &screen)
{
	Private::copy(&_private, screen._private);

	return *this;
}

} // namespace Yttrium
