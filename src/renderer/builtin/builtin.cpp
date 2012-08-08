#include "builtin.h"

#include "../renderer.h"

#include "data.h"

namespace Yttrium
{

void RendererBuiltin::Private::bind()
{
	if (!_is_bound)
	{
		_renderer->flush_2d();

		// TODO: Clean up all texture-dependent resources.

		_renderer->bind_builtin();
		_renderer->set_matrix_2d(_renderer->_viewport_size.width, _renderer->_viewport_size.height);
		_is_bound = true;
	}
}

RendererBuiltin::RendererBuiltin(const RendererBuiltin &renderer)
	: _private(renderer._private)
{
	_private->_renderer = Renderer::Private::copy(_private->_renderer);
}

RendererBuiltin::~RendererBuiltin()
{
	Renderer::Private::release(&_private->_renderer);
}

void RendererBuiltin::draw_cursor(Dim x, Dim y)
{
	_private->bind();

	_private->_renderer->draw_rectangle(
		Rectf(
			x * Builtin::char_width, y * Builtin::char_height,
			Builtin::char_width, Builtin::char_height),
		Rectf::from_outer_coords(
			Builtin::coords[0][0][0], Builtin::coords[0][0][1],
			Builtin::coords[0][1][0], Builtin::coords[0][1][1]));
}

void RendererBuiltin::draw_image(Dim x, Dim y, Dim width, Dim height)
{
	_private->bind();

	_private->_renderer->draw_rectangle(
		Rectf(
			x * Builtin::char_width, y * Builtin::char_height,
			width * Builtin::char_width, height * Builtin::char_height),
		Rectf::from_outer_coords(
			Builtin::coords[0][0][0], Builtin::coords[0][0][0],
			Builtin::coords[0][0][0], Builtin::coords[0][0][0]));
}

void RendererBuiltin::draw_text(Dim x, Dim y, const StaticString &text, Dim max_size)
{
	_private->bind();

	Dim size = min<Dim>(text.size(), max_size);

	if (size > 0)
	{
		// We can't join the text in a single strip because the
		// adjacent letters may use different texture coordinates.

		Vector2f a(x * Builtin::char_width, y * Builtin::char_height);

		for (const UChar *symbol = reinterpret_cast<const UChar *>(text.text()); size; --size, ++symbol)
		{
			if (*symbol >= Builtin::first_char && *symbol <= Builtin::last_char)
			{
				if (*symbol != Builtin::first_char) // Don't render spaces.
				{
					_private->_renderer->draw_rectangle(
						Rectf(
							a.x, a.y,
							Builtin::char_width, Builtin::char_height),
						Rectf::from_outer_coords(
							Builtin::coords[*symbol][0][0], Builtin::coords[*symbol][0][1],
							Builtin::coords[*symbol][1][0], Builtin::coords[*symbol][1][1]));
				}

				a.x += Builtin::char_width;
			}
		}
	}
}

void RendererBuiltin::set_color(const Vector4f &color)
{
	_private->_renderer->_color = color;
}

Dim2 RendererBuiltin::size() const
{
	const Dim2 &viewport_size = _private->_renderer->_viewport_size;

	return Dim2(
		viewport_size.width / Builtin::char_width,
		viewport_size.height / Builtin::char_height);
}

Dim2 RendererBuiltin::text_size(const StaticString &text) const
{
	return Dim2(Builtin::char_width * text.size(), Builtin::char_height);
}

RendererBuiltin &RendererBuiltin::operator =(const RendererBuiltin &renderer)
{
	Renderer::Private::release(&_private->_renderer);

	_private = renderer._private;
	_private->_renderer = Renderer::Private::copy(_private->_renderer);

	return *this;
}

RendererBuiltin::RendererBuiltin(Private *private_)
	: _private(private_)
{
	_private->_renderer = Renderer::Private::copy(_private->_renderer);
}

} // namespace Yttrium
