/// \file
/// \brief

#ifndef __Y_RENDERER_H
#define __Y_RENDERER_H

#include <Yttrium/allocator.h>
#include <Yttrium/renderer/builtin.h>
#include <Yttrium/static_string.h>
#include <Yttrium/vector.h>

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

	Renderer() noexcept
		: _private(nullptr)
	{
	}

	///

	Renderer(const Renderer &renderer) noexcept;

	///

	~Renderer() noexcept;

public:

	/// Clear the framebuffer and begin a new frame.

	void begin_frame() noexcept;

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

	void set_viewport(Dim x, Dim y, Dim width, Dim height) noexcept;

	/// Take a screenshot.
	/// \param name
	/// \note The screenshot would be actually taken at the end of the frame
	/// and saved in the PNG format.

	void take_screenshot(const StaticString &name) noexcept;

	///

	Dim2 viewport_size() const noexcept;

public:

	///

	operator bool() const noexcept
	{
		return _private;
	}

	///

	Renderer &operator =(const Renderer &renderer) noexcept;

public:

	class Private;

private:

	Private *_private;

private:

	Y_PRIVATE Renderer(Window *window, Backend backend, Allocator *allocator);
};

} // namespace Yttrium

#endif // __Y_RENDERER_H
