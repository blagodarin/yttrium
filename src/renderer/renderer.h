#ifndef __RENDERER_RENDERER_H
#define __RENDERER_RENDERER_H

#include <Yttrium/buffer.h>
#include <Yttrium/rect.h>
#include <Yttrium/renderer.h>
#include <Yttrium/string.h>
#include <Yttrium/window.h>

#include "../base/private_base.h"

#include "builtin/builtin.h"

#include <vector>

namespace Yttrium
{

class Y_PRIVATE Renderer::Private: public PrivateBase<Renderer::Private>
{
public:

	Private(Window *window, Allocator *allocator);

	virtual ~Private();

public:

	virtual void bind_builtin() = 0;

	virtual void clear() = 0;

	virtual void flush_2d() = 0;

	virtual void set_matrix_2d(double width, double height) = 0;

	virtual void set_viewport(Dim x, Dim y, Dim width, Dim height) = 0;

	virtual void take_screenshot() = 0;

public:

	void draw_rectangle(const Rectf &position, const Rectf &texture);

public:

	Window _window; // Don't let the window die too early.

	Dim2     _viewport_size;
	Vector2d _rendering_size;

	String _screenshot_filename;
	Buffer _screenshot_buffer;

	struct Vertex2D
	{
		Vector2f position;
		Vector4f color;
		Vector2f texture;
	};

	typedef std::vector<Vertex2D> Vertices2D;
	typedef std::vector<int16_t>  Indices2D;

	Vertices2D _vertices_2d;
	Indices2D  _indices_2d;

	RendererBuiltin::Private _builtin;

	Vector4f _color;

public:

	static Private *create(Window *window, Renderer::Backend backend, Allocator *allocator);
};

} // namespace Yttrium

#endif // __RENDERER_RENDERER_H
