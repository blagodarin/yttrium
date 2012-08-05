#include "window.h"

#include <Yttrium/assert.h>

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

Renderer Window::create_renderer(Renderer::Backend backend, Allocator *allocator)
{
	if (!_private || _private->_renderer
		|| !is_supported(backend))
	{
		return Renderer();
	}

	return Renderer(this, backend, allocator ? allocator : _private->_allocator);
}

Window &Window::operator =(const Window &window)
{
	close();

	_private = Private::copy(window._private);

	return *this;
}

bool Window::is_supported(Renderer::Backend backend)
{
	return backend == Renderer::OpenGl;
}

} // namespace Yttrium
