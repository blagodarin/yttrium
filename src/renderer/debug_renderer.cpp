#include "debug_renderer.h"

#include "builtin_data.h"
#include "renderer.h"

namespace Yttrium
{
	DebugRenderer::DebugRenderer(Renderer& renderer)
		: _renderer(Renderer::Private::get(renderer))
		, _color(1, 1, 1)
	{
		// NOTE: Copypasted from "renderer/renderer.cpp".

		if (_renderer._vertices_2d.size())
		{
			_renderer.flush_2d();
			_renderer._vertices_2d.clear();
			_renderer._indices_2d.clear();
		}

		_renderer._texture.reset();
		// TODO: Reset font.

		_renderer.bind_debug_texture();
		_renderer.set_matrix_2d(_renderer._viewport_size.x, _renderer._viewport_size.y);

		_renderer._debug_rendering = true;
	}

	DebugRenderer::~DebugRenderer()
	{
		_renderer._debug_rendering = true;
	}

	void DebugRenderer::draw_cursor(int x, int y)
	{
		_renderer._color = _color;
		_renderer.draw_rectangle(
			RectF(
				x * Builtin::char_width, y * Builtin::char_height,
				Builtin::char_width, Builtin::char_height),
			RectF::from_coords(
				Builtin::coords[0][0][0], Builtin::coords[0][0][1],
				Builtin::coords[0][1][0], Builtin::coords[0][1][1]));
	}

	void DebugRenderer::draw_rectangle(int x, int y, int width, int height)
	{
		_renderer._color = _color;
		_renderer.draw_rectangle(
			RectF(
				x * Builtin::char_width, y * Builtin::char_height,
				width * Builtin::char_width, height * Builtin::char_height),
			RectF::from_coords(
				Builtin::coords[0][0][0], Builtin::coords[0][0][0],
				Builtin::coords[0][0][0], Builtin::coords[0][0][0]));
	}

	void DebugRenderer::draw_text(int x, int y, const StaticString& text, int max_size)
	{
		int size = std::min<int>(text.size(), max_size);
		if (size <= 0)
			return;

		_renderer._color = _color;

		// We can't join the text in a single strip because the
		// adjacent letters may use different texture coordinates.

		Vector2f a(x * Builtin::char_width, y * Builtin::char_height);

		for (const unsigned char* symbol = reinterpret_cast<const unsigned char*>(text.text()); size; --size, ++symbol)
		{
			if (*symbol >= Builtin::first_char && *symbol <= Builtin::last_char)
			{
				if (*symbol != Builtin::first_char) // Don't render spaces.
				{
					_renderer.draw_rectangle(
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
	}

	Dim2 DebugRenderer::size() const
	{
		const Dim2& viewport_size = _renderer._viewport_size;
		return Dim2(viewport_size.x / Builtin::char_width, viewport_size.y / Builtin::char_height);
	}

	Dim2 DebugRenderer::text_size(const StaticString& text) const
	{
		return Dim2(Builtin::char_width * text.size(), Builtin::char_height);
	}
}
