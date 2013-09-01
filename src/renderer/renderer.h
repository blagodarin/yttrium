#ifndef __RENDERER_RENDERER_H
#define __RENDERER_RENDERER_H

#include <yttrium/image.h>
#include <yttrium/margins.h>
#include <yttrium/rect.h>
#include <yttrium/renderer.h>
#include <yttrium/string.h>
#include <yttrium/window.h>

#include "../base/private_base.h"
#include "builtin.h"

#include <vector>

namespace Yttrium
{

class Y_PRIVATE Renderer::Private
	: public PrivateBase<Renderer::Private>
{
public:

	Private(Window *window, Allocator *allocator);

	virtual ~Private();

public:

	virtual void bind_builtin() = 0;

	virtual void clear() = 0;

	virtual void flush_2d() = 0;

	virtual void set_matrix_2d(double width, double height) = 0;

	virtual void set_viewport(const Dim2 &size);

	virtual void take_screenshot() = 0;

public:

	void draw_rectangle(const RectF &position, const RectF &texture, const MarginsF &borders = MarginsF());

	void draw_text(const Vector2f &position, const StaticString &text, Alignment alignment);

	Vector2f text_size(const StaticString &text) const;

public:

	Window _window; // Don't let the window die too early.

	Dim2     _viewport_size;
	Vector2d _rendering_size;

	String _screenshot_filename;
	Image  _screenshot_image;

	Vector4f _color;

	// 2D rendering.

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

	Texture2DPtr _texture;
	RectF        _texture_rect;
	MarginsF     _texture_borders;

	TextureFont _font;
	Vector2f    _font_size;

	// Subinterfaces.

	RendererBuiltin::Private _builtin;

public:

	static Private *create(Window *window, Renderer::Backend backend, Allocator *allocator);
};

} // namespace Yttrium

#endif // __RENDERER_RENDERER_H
