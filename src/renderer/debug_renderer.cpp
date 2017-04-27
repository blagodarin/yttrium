#include "debug_renderer.h"

#include <yttrium/renderer/texture.h>
#include "debug_texture.h"
#include "renderer.h"

#include <algorithm>

namespace
{
	using namespace Yttrium;

	void draw_debug_char(RendererImpl& renderer, int x, int y, int width, int height, const Color4f& color, uint8_t value)
	{
		renderer.draw_rect(
			{
				Vector2(x * DebugTexture::char_width, y * DebugTexture::char_height),
				SizeF(width * DebugTexture::char_width, height * DebugTexture::char_height)
			},
			color,
			{
				Vector2(DebugTexture::coords[value][0][0], DebugTexture::coords[value][0][1]),
				Vector2(DebugTexture::coords[value][1][0], DebugTexture::coords[value][1][1])
			});
	}
}

namespace Yttrium
{
	DebugRenderer::DebugRenderer(RendererImpl& renderer)
		: _renderer{renderer}
		, _debug_texture{_renderer, _renderer.debug_texture(), Texture2D::NearestFilter}
	{
	}

	void DebugRenderer::draw_cursor(int x, int y)
	{
		::draw_debug_char(_renderer, x, y, 1, 1, _color, DebugTexture::cursor_index);
	}

	void DebugRenderer::draw_rectangle(int x, int y, int width, int height)
	{
		::draw_debug_char(_renderer, x, y, width, height, _color, DebugTexture::rect_index);
	}

	void DebugRenderer::draw_text(int x, int y, const StaticString& text, const boost::optional<size_t>& max_size)
	{
		const auto size = max_size ? std::min(text.size(), *max_size) : text.size();
		for (size_t i = 0; i < size; ++i)
		{
			const auto symbol = static_cast<uint8_t>(text[i]);
			if (symbol >= DebugTexture::first_char && symbol <= DebugTexture::last_char)
				::draw_debug_char(_renderer, x + static_cast<int>(i), y, 1, 1, _color, symbol);
		}
	}

	size_t DebugRenderer::max_width() const
	{
		return static_cast<size_t>(_renderer.window_size()._width) / DebugTexture::char_width;
	}

	void DebugRenderer::set_color(float r, float g, float b, float a)
	{
		_color = {r, g, b, a};
	}
}
