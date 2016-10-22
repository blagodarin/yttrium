#include "debug_renderer.h"

#include <yttrium/renderer/texture.h>
#include "debug_texture.h"
#include "renderer.h"

namespace
{
	using namespace Yttrium;

	void draw_debug_char(RendererImpl& renderer, int x, int y, int width, int height, uint8_t value)
	{
		renderer.draw_rect(
			{
				PointF(x * DebugTexture::char_width, y * DebugTexture::char_height),
				SizeF(width * DebugTexture::char_width, height * DebugTexture::char_height)
			},
			{
				PointF(DebugTexture::coords[value][0][0], DebugTexture::coords[value][0][1]),
				PointF(DebugTexture::coords[value][1][0], DebugTexture::coords[value][1][1])
			});
	}
}

namespace Yttrium
{
	DebugRenderer::DebugRenderer(RendererImpl& renderer)
		: _renderer(renderer)
		, _debug_texture(_renderer, _renderer.debug_texture(), Texture2D::NearestFilter)
	{
		set_color(1, 1, 1);
	}

	void DebugRenderer::draw_cursor(int x, int y)
	{
		::draw_debug_char(_renderer, x, y, 1, 1, DebugTexture::cursor_index);
	}

	void DebugRenderer::draw_rectangle(int x, int y, int width, int height)
	{
		::draw_debug_char(_renderer, x, y, width, height, DebugTexture::rect_index);
	}

	void DebugRenderer::draw_text(int x, int y, const StaticString& text, int max_size)
	{
		const int size = max_size < 0 ? text.size() : std::min<int>(text.size(), max_size);
		for (int i = 0; i < size; ++i)
		{
			const uint8_t symbol = text[i];
			if (symbol >= DebugTexture::first_char && symbol <= DebugTexture::last_char)
				::draw_debug_char(_renderer, x + i, y, 1, 1, symbol);
		}
	}

	int DebugRenderer::max_width() const
	{
		return _renderer.window_size().width() / DebugTexture::char_width;
	}

	void DebugRenderer::set_color(float r, float g, float b, float a)
	{
		_renderer.set_color({r, g, b, a});
	}
}
