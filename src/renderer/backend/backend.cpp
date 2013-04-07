#include "gl/renderer.h"

namespace Yttrium
{

Renderer::Private *Renderer::Private::create(Window *window, Renderer::Backend backend, Allocator *allocator)
{
	Renderer::Private *result = nullptr;

	switch (backend)
	{
	case Renderer::OpenGl:

		result = Y_NEW(allocator, OpenGlRenderer)(window, allocator);
		break;

	default:

		break;
	}

	if (!result)
	{
		Y_ABORT("Can't create renderer"); // NOTE: Safe to continue.
	}

	return result;
}

} // namespace Yttrium
