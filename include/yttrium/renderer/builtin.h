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

	RendererBuiltin(const RendererBuiltin &renderer);

	///

	~RendererBuiltin();

public:

	///

	Vector4f color() const;

	///

	void draw_cursor(int x, int y);

	///

	void draw_rectangle(int x, int y, int width, int height);

	///

	void draw_text(int x, int y, const StaticString &text, int max_size = -1);

	/// Set the drawing color to \a color.

	void set_color(const Vector4f &color);

	/**
	* \overload
	*/

	void set_color(float r, float g, float b, float a = 1)
	{
		set_color(Vector4f(r, g, b, a));
	}

	///

	Dim2 size() const; // TODO: Rename.

	/// Calculate the \a text size in builtin units.

	Dim2 text_size(const StaticString &text) const;

public:

	///

	inline operator bool() const
	{
		return _private;
	}

	///

	RendererBuiltin &operator =(const RendererBuiltin &renderer);

private:

	class Private;

	Private *_private;

	Y_PRIVATE RendererBuiltin(Private *private_);
};


} // namespace Yttrium

#endif // __Y_RENDERER_BUILTIN_H
