/// \file
/// \brief Built-in rendering interface.

#ifndef __Y_RENDERER_BUILTIN_H
#define __Y_RENDERER_BUILTIN_H

#include <Yttrium/static_string.h>
#include <Yttrium/vector.h>

namespace Yttrium
{

class Renderer;

///

class Y_API RendererBuiltin
{
	friend class Renderer;

public:

	///

	RendererBuiltin(const RendererBuiltin &renderer) noexcept;

	///

	~RendererBuiltin() noexcept;

public:

	///

	Vector4f color() const noexcept;

	///

	void draw_cursor(Dim x, Dim y) noexcept;

	///

	void draw_rectangle(Dim x, Dim y, Dim width, Dim height) noexcept;

	///

	void draw_text(Dim x, Dim y, const StaticString &text, Dim max_size = -1) noexcept;

	/// Set the drawing color to \a color.

	void set_color(const Vector4f &color) noexcept;

	/**
	* \overload
	*/

	inline void set_color(float r, float g, float b, float a = 1) noexcept;

	///

	Dim2 size() const noexcept;

	/// Calculate the \a text size in builtin units.

	Dim2 text_size(const StaticString &text) const noexcept;

public:

	///

	inline operator bool() const noexcept;

	///

	RendererBuiltin &operator =(const RendererBuiltin &renderer) noexcept;

private:

	class Private;

private:

	Y_PRIVATE RendererBuiltin(Private *private_);

private:

	Private *_private;
};

////////////////////////////////////////////////////////////////////////////////

RendererBuiltin::operator bool() const noexcept
{
	return _private;
}

void RendererBuiltin::set_color(float r, float g, float b, float a) noexcept
{
	set_color(Vector4f(r, g, b, a));
}

} // namespace Yttrium

#endif // __Y_RENDERER_BUILTIN_H
