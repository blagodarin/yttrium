/// \file
/// \brief Built-in rendering interface.

#ifndef __Y_RENDERER_BUILTIN_H
#define __Y_RENDERER_BUILTIN_H

#include <yttrium/static_string.h>
#include <yttrium/vector.h>

namespace Yttrium
{

class Renderer;

///

class Y_API RendererBuiltin
{
	friend Renderer;

public:

	///

	RendererBuiltin(const RendererBuiltin &renderer) noexcept;

	///

	~RendererBuiltin() noexcept;

public:

	///

	Vector4f color() const noexcept;

	///

	void draw_cursor(int x, int y) noexcept;

	///

	void draw_rectangle(int x, int y, int width, int height) noexcept;

	///

	void draw_text(int x, int y, const StaticString &text, int max_size = -1) noexcept;

	/// Set the drawing color to \a color.

	void set_color(const Vector4f &color) noexcept;

	/**
	* \overload
	*/

	void set_color(float r, float g, float b, float a = 1) noexcept
	{
		set_color(Vector4f(r, g, b, a));
	}

	///

	Dim2 size() const noexcept; // TODO: Rename.

	/// Calculate the \a text size in builtin units.

	Dim2 text_size(const StaticString &text) const noexcept;

public:

	///

	inline operator bool() const noexcept
	{
		return _private;
	}

	///

	RendererBuiltin &operator =(const RendererBuiltin &renderer) noexcept;

private:

	class Private;

	Private *_private;

	Y_PRIVATE RendererBuiltin(Private *private_);
};


} // namespace Yttrium

#endif // __Y_RENDERER_BUILTIN_H
