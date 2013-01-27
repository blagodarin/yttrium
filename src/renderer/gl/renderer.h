#ifndef __RENDERER_GL_RENDERER_H
#define __RENDERER_GL_RENDERER_H

#include "../renderer.h"

#include "gl.h"
#include "texture_cache.h"

namespace Yttrium
{

class OpenGlRenderer: public Renderer::Private
{
public:

	OpenGlRenderer(Window *window, Allocator *allocator);
	virtual ~OpenGlRenderer();

public:

	bool check_version(int major, int minor);

public: // Renderer::Private

	virtual void bind_builtin();
	virtual void clear();
	virtual void flush_2d();
	virtual void set_matrix_2d(double width, double height);
	virtual void set_viewport(const Dim2 &size);
	virtual void take_screenshot();

public:

	GlApi  _gl;
	GLuint _builtin_texture;
};

} // namespace Yttrium

#endif // __RENDERER_GL_RENDERER_H
