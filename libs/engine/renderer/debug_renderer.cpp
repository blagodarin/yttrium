#include "debug_renderer.h"

#include "builtin/builtin.h"
#include "builtin/debug_texture.h"
#include "pass.h"

#include <algorithm>

namespace
{
	using namespace Yttrium;

	void draw_debug_char(RenderPassImpl& pass, size_t x, size_t y, size_t width, size_t height, const Color4f& color, uint8_t value)
	{
		pass.draw_rect(
			{ { static_cast<float>(x * DebugTexture::char_width), static_cast<float>(y * DebugTexture::char_height) },
				SizeF{ static_cast<float>(width * DebugTexture::char_width), static_cast<float>(height * DebugTexture::char_height) } },
			color,
			{ { DebugTexture::coords[value][0][0], DebugTexture::coords[value][0][1] },
				Vector2{ DebugTexture::coords[value][1][0], DebugTexture::coords[value][1][1] } });
	}
}

namespace Yttrium
{
	DebugRenderer::DebugRenderer(RenderPass& pass)
		: _pass{ static_cast<RenderPassImpl&>(pass) }
		, _texture{ _pass, _pass.builtin()._debug_texture.get(), Texture2D::NearestFilter }
	{
	}

	void DebugRenderer::draw_cursor(size_t x, size_t y)
	{
		::draw_debug_char(_pass, x, y, 1, 1, _color, DebugTexture::cursor_index);
	}

	void DebugRenderer::draw_rectangle(size_t x, size_t y, size_t width, size_t height)
	{
		::draw_debug_char(_pass, x, y, width, height, _color, DebugTexture::rect_index);
	}

	void DebugRenderer::draw_text(size_t x, size_t y, std::string_view text, const std::optional<size_t>& max_size)
	{
		const auto size = max_size ? std::min(text.size(), *max_size) : text.size();
		for (size_t i = 0; i < size; ++i)
		{
			const auto symbol = static_cast<uint8_t>(text[i]);
			if (symbol >= DebugTexture::first_char && symbol <= DebugTexture::last_char)
				::draw_debug_char(_pass, x + i, y, 1, 1, _color, symbol);
		}
	}

	size_t DebugRenderer::max_width() const
	{
		return static_cast<size_t>(_pass.window_size()._width) / DebugTexture::char_width;
	}

	void DebugRenderer::set_color(float r, float g, float b, float a)
	{
		_color = { r, g, b, a };
	}
}
