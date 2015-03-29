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

class RendererImpl: public Renderer
{
	friend DebugRenderer;

public:

	static std::unique_ptr<RendererImpl> create(WindowBackend& window, Allocator* allocator);

	RendererImpl(WindowBackend& window, Allocator* allocator);

	Allocator* allocator() const override;
	void draw_rectangle(const RectF& rect) override;
	void draw_rectangle(const RectF& rect, const RectF& texture_rect) override;
	void draw_text(const Vector2f& position, const StaticString& text, unsigned alignment, TextCapture* capture) override;
	void end_frame() override;
	void flush_2d() override;
	Vector2d rendering_size() const override;
	void set_color(const Vector4f& color) override;
	bool set_font(const TextureFont& font) override;
	void set_font_size(const Vector2f& size) override;
	void set_matrix_2d(double width, double height) override;
	void set_matrix_2d_height(double height) override;
	void set_matrix_2d_width(double width) override;
	void set_texture(const Texture2DPtr& texture) override;
	bool set_texture_borders(const MarginsI& borders) override;
	void set_texture_rectangle(const RectF& rect) override;
	void take_screenshot(const StaticString& name) override;
	Vector2f text_size(const StaticString& text) const override;
	Dim2 viewport_size() const override;

	virtual bool initialize() = 0;

	virtual void clear() = 0;
	virtual void do_flush_2d() = 0;
	virtual void do_set_matrix_2d(double width, double height) = 0;
	virtual void set_viewport(const Dim2 &size);
	virtual void take_screenshot() = 0;

protected:

	virtual void bind_debug_texture() = 0;

public:

	void draw_rectangle(const RectF &position, const RectF &texture, const MarginsF &borders);

public:

	Allocator* const _allocator;
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
};

} // namespace Yttrium

#endif // __RENDERER_RENDERER_H
