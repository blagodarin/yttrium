/// \file
/// \brief

#ifndef __Y_RENDERER_H
#define __Y_RENDERER_H

#include <yttrium/rect.h>
#include <yttrium/renderer/builtin.h>
#include <yttrium/renderer/pointers.h>
#include <yttrium/static_string.h>
#include <yttrium/texture_font.h>

namespace Yttrium
{

class Window;

///

class Y_API Renderer
{
	friend TextureCache;
	friend Window;

public:

	///

	enum Backend
	{
		Software, ///< Software backend.
		OpenGl,   ///< OpenGL backend.
	};

public:

	///

	inline Renderer() noexcept;

	///

	Renderer(const Renderer &renderer) noexcept;

	///

	~Renderer() noexcept;

public:

	///

	Allocator *allocator() const noexcept;

	/// Clear the framebuffer and begin a new frame.

	void begin_frame() noexcept;

	///

	void draw_rectangle(const RectF &rect) noexcept;

	///

	inline void draw_rectangle(float x, float y, float width, float height) noexcept;

	///

	void draw_rectangle(const RectF &rect, const RectF &texture_rect) noexcept;

	///

	void draw_text(const Vector2f &position, const StaticString &text, Alignment alignment = BottomRightAlignment) noexcept;

	///

	inline void draw_text(float x, float y, const StaticString &text, Alignment alignment = BottomRightAlignment) noexcept;

	/// Finish the rendering frame, swap the framebuffers, reset the rendering mode
	/// and, if requested, capture a screenshot.

	void end_frame() noexcept;

	///

	void flush_2d() noexcept;

	///

	RendererBuiltin renderer_builtin() noexcept;

	///

	Vector2d rendering_size() const noexcept;

	///

	void set_color(const Vector4f &color) noexcept;

	///

	inline void set_color(float r, float g, float b, float a = 1.f) noexcept;

	///

	bool set_font(const TextureFont &font) noexcept;

	///

	void set_font_size(const Vector2f &size) noexcept;

	///

	inline void set_font_size(float y_size, float x_scaling = 1) noexcept;

	///

	void set_matrix_2d(double width, double height) noexcept;

	///

	void set_matrix_2d_height(double height) noexcept;

	///

	void set_matrix_2d_width(double width) noexcept;

	///

	void set_texture(const Texture2DPtr &texture) noexcept;

	///

	bool set_texture_borders(const MarginsI &borders) noexcept;

	///

	void set_texture_rectangle(const RectF &rect) noexcept;

	///

	inline void set_texture_rectangle(float x, float y, float width, float height) noexcept;

	/// Take a screenshot.
	/// \param name
	/// \note The screenshot would be actually taken at the end of the frame
	/// and saved in the PNG format.

	void take_screenshot(const StaticString &name) noexcept;

	///

	Vector2f text_size(const StaticString &text) const noexcept;

	///

	Dim2 viewport_size() const noexcept;

public:

	///

	inline operator bool() const noexcept;

	///

	Renderer &operator =(const Renderer &renderer) noexcept;

public:

	class Private;

private:

	Private *_private;

private:

	Y_PRIVATE Renderer(Window *window, Backend backend, Allocator *allocator);
	Y_PRIVATE Renderer(Private *private_);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Renderer::Renderer() noexcept
	: _private(nullptr)
{
}

void Renderer::draw_rectangle(float x, float y, float width, float height) noexcept
{
	draw_rectangle(RectF(x, y, width, height));
}

void Renderer::draw_text(float x, float y, const StaticString &text, Alignment alignment) noexcept
{
	draw_text(Vector2f(x, y), text, alignment);
}

void Renderer::set_color(float r, float g, float b, float a) noexcept
{
	set_color(Vector4f(r, g, b, a));
}

void Renderer::set_font_size(float y_size, float x_scaling) noexcept
{
	set_font_size(Vector2f(x_scaling, y_size));
}

void Renderer::set_texture_rectangle(float x, float y, float width, float height) noexcept
{
	set_texture_rectangle(RectF(x, y, width, height));
}

Renderer::operator bool() const noexcept
{
	return _private;
}

} // namespace Yttrium

#endif // __Y_RENDERER_H
