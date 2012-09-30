/// \file
/// \brief

#ifndef __Y_RENDERER_H
#define __Y_RENDERER_H

#include <Yttrium/rect.h>
#include <Yttrium/renderer/builtin.h>
#include <Yttrium/renderer/texture_cache.h>
#include <Yttrium/static_string.h>

namespace Yttrium
{

class Window;

///

class Y_API Renderer
{
	friend class Window;

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

	/// Clear the framebuffer and begin a new frame.

	void begin_frame() noexcept;

	///

	void draw_rectangle(const RectF &rect) noexcept;

	///

	inline void draw_rectangle(float x, float y, float width, float height) noexcept;

	///

	void draw_rectangle(const RectF &rect, const RectF &texture_rect) noexcept;

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

	void set_matrix_2d(double width, double height) noexcept;

	///

	void set_matrix_2d_height(double height) noexcept;

	///

	void set_matrix_2d_width(double width) noexcept;

	///

	void set_texture(const Texture2D &texture) noexcept;

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

	TextureCache texture_cache() noexcept;

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
