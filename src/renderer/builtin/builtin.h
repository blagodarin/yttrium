#ifndef __RENDERER_BUILTIN_BUILTIN_H
#define __RENDERER_BUILTIN_BUILTIN_H

#include <Yttrium/renderer.h>

namespace Yttrium
{

class Y_PRIVATE RendererBuiltin::Private
{
public:

	Renderer::Private *_renderer;
	bool               _is_bound;

public:

	Private()
		: _renderer(nullptr)
		, _is_bound(false)
	{
	}

public:

	void bind();
};

} // namespace Yttrium

#endif // __RENDERER_BUILTIN_BUILTIN_H
