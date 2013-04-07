#ifndef __RENDERER_BUILTIN_H
#define __RENDERER_BUILTIN_H

#include <Yttrium/renderer.h>

namespace Yttrium
{

class Y_PRIVATE RendererBuiltin::Private
{
public:

	Renderer::Private *_renderer;
	bool               _is_bound;
	Vector4f           _color;

public:

	Private();

public:

	void bind();
};

} // namespace Yttrium

#endif // __RENDERER_BUILTIN_H
