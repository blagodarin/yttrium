#include "builtin.h"

#include "builtin_data.h"
#include "renderer.h"

#include <algorithm> // min

namespace Yttrium
{

RendererBuiltin::Private::Private()
	: _renderer(nullptr)
	, _is_bound(false)
	, _color(1, 1, 1)
{
}

void RendererBuiltin::Private::bind()
{
	if (!_is_bound)
	{
		// NOTE: Copypasted from "renderer/renderer.cpp".

		if (_renderer->_vertices_2d.size())
		{
			_renderer->flush_2d();
			_renderer->_vertices_2d.clear();
			_renderer->_indices_2d.clear();
		}

		_renderer->_texture.reset();
		// TODO: Reset font.

		_renderer->bind_builtin();
		_renderer->set_matrix_2d(_renderer->_viewport_size.x, _renderer->_viewport_size.y);
		_is_bound = true;
	}
}

RendererBuiltin::RendererBuiltin(const RendererBuiltin& renderer)
	: _private(renderer._private)
{
	Renderer::Private::copy(_private->_renderer);
}

RendererBuiltin::~RendererBuiltin()
{
	// Don't clean up the renderer pointer, it may be in use by another RendererBuiltin instance.

	Renderer::Private::release(_private->_renderer);
}

Vector4f RendererBuiltin::color() const
{
	return _private->_color;
}

void RendererBuiltin::draw_cursor(int x, int y)
{
	_private->bind();

	Vector4f old_color = _private->_renderer->_color;

	_private->_renderer->_color = _private->_color;

	_private->_renderer->draw_rectangle(
		RectF(
			x * Builtin::char_width, y * Builtin::char_height,
			Builtin::char_width, Builtin::char_height),
		RectF::from_coords(
			Builtin::coords[0][0][0], Builtin::coords[0][0][1],
			Builtin::coords[0][1][0], Builtin::coords[0][1][1]));

	_private->_renderer->_color = old_color;
}

void RendererBuiltin::draw_rectangle(int x, int y, int width, int height)
{
	_private->bind();

	Vector4f old_color = _private->_renderer->_color;

	_private->_renderer->_color = _private->_color;

	_private->_renderer->draw_rectangle(
		RectF(
			x * Builtin::char_width, y * Builtin::char_height,
			width * Builtin::char_width, height * Builtin::char_height),
		RectF::from_coords(
			Builtin::coords[0][0][0], Builtin::coords[0][0][0],
			Builtin::coords[0][0][0], Builtin::coords[0][0][0]));

	_private->_renderer->_color = old_color;
}

void RendererBuiltin::draw_text(int x, int y, const StaticString& text, int max_size)
{
	_private->bind();

	int size = std::min<int>(text.size(), max_size);

	if (size > 0)
	{
		Vector4f old_color = _private->_renderer->_color;

		_private->_renderer->_color = _private->_color;

		// We can't join the text in a single strip because the
		// adjacent letters may use different texture coordinates.

		Vector2f a(x * Builtin::char_width, y * Builtin::char_height);

		for (const unsigned char* symbol = reinterpret_cast<const unsigned char*>(text.text()); size; --size, ++symbol)
		{
			if (*symbol >= Builtin::first_char && *symbol <= Builtin::last_char)
			{
				if (*symbol != Builtin::first_char) // Don't render spaces.
				{
					_private->_renderer->draw_rectangle(
						RectF(
							a.x, a.y,
							Builtin::char_width, Builtin::char_height),
						RectF::from_coords(
							Builtin::coords[*symbol][0][0], Builtin::coords[*symbol][0][1],
							Builtin::coords[*symbol][1][0], Builtin::coords[*symbol][1][1]));
				}

				a.x += Builtin::char_width;
			}
		}

		_private->_renderer->_color = old_color;
	}
}

void RendererBuiltin::set_color(const Vector4f& color)
{
	_private->_color = color;
}

Dim2 RendererBuiltin::size() const
{
	const Dim2 &viewport_size = _private->_renderer->_viewport_size;

	return Dim2(
		viewport_size.x / Builtin::char_width,
		viewport_size.y / Builtin::char_height);
}

Dim2 RendererBuiltin::text_size(const StaticString& text) const
{
	return Dim2(Builtin::char_width * text.size(), Builtin::char_height);
}

RendererBuiltin& RendererBuiltin::operator=(const RendererBuiltin& renderer)
{
	if (_private != renderer._private)
	{
		Renderer::Private::release(&_private->_renderer);
		_private = renderer._private;
		_private->_renderer = Renderer::Private::copy(_private->_renderer);
	}

	return *this;
}

RendererBuiltin::RendererBuiltin(Private* private_)
	: _private(private_)
{
	_private->_renderer = Renderer::Private::copy(_private->_renderer);
}

} // namespace Yttrium
