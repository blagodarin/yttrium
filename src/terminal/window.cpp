#include "window.h"

namespace Yttrium
{

Window::Window(const Window &window)
	: _private(Private::copy(window._private))
	, _callbacks(window._callbacks)
{
}

void Window::close()
{
	Private::release(&_private);
}

Window &Window::operator =(const Window &window)
{
	close();

	_private = Private::copy(window._private);

	return *this;
}

} // namespace Yttrium
