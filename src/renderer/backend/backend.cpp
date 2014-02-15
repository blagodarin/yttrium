#include "gl/renderer.h"

#include "../../memory/allocatable.h"

namespace Yttrium
{

Renderer::Private *Renderer::Private::create(Window *window, Renderer::Backend backend, Allocator *allocator)
{
	Allocatable<Renderer::Private> renderer(allocator);

	switch (backend)
	{
	case Renderer::OpenGl:

		renderer.reset<OpenGlRenderer>(window);
		break;

	default:

		break;
	}

	if (!renderer)
	{
		Y_ABORT("Can't create renderer"); // NOTE: Safe to continue.
	}

	return renderer.release();
}

} // namespace Yttrium
