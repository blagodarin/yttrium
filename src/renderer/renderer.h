#ifndef __RENDERER_RENDERER_H
#define __RENDERER_RENDERER_H

#include <yttrium/image.h>
#include <yttrium/margins.h>
#include <yttrium/rect.h>
#include <yttrium/renderer.h>
#include <yttrium/string.h>

#include "../base/private_base.h"
#include "../window/backend.h"

#include <vector>

namespace Yttrium
{

class DebugRenderer;

class Y_PRIVATE Renderer::Private: public PrivateBase<Renderer::Private>
{
	friend DebugRenderer;

public:

	Private(WindowBackend& window, Allocator* allocator);
	virtual ~Private();

public:

	virtual bool initialize() = 0;
	virtual void bind_debug_texture() = 0;
	virtual void clear() = 0;
	virtual std::unique_ptr<TextureCache> create_texture_cache(Renderer&) = 0;
	virtual void flush_2d() = 0;
	virtual void set_matrix_2d(double width, double height) = 0;
	virtual void set_viewport(const Dim2 &size);
	virtual void take_screenshot() = 0;

public:

	void draw_rectangle(const RectF &position, const RectF &texture, const MarginsF &borders = MarginsF());

	void draw_text(const Vector2f &position, const StaticString &text, unsigned alignment, TextCapture* capture);

	Vector2f text_size(const StaticString &text) const;

public:

	WindowBackend& _window;

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

	std::vector<Vertex2D> _vertices_2d;
	std::vector<int16_t>  _indices_2d;

	Texture2DPtr _texture;
	RectF        _texture_rect;
	MarginsF     _texture_borders;

	TextureFont _font;
	Vector2f    _font_size;

	bool _debug_rendering = false;

public:

	static Private* create(WindowBackend& window, Allocator* allocator);

private:

	static Private& get(Renderer& renderer)
	{
		return *renderer._private;
	}
};

} // namespace Yttrium

#endif // __RENDERER_RENDERER_H
