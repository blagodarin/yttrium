// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "debug_renderer.h"

#include "builtin.h"
#include "debug_texture.h"
#include "pass.h"

#include <algorithm>

namespace
{
	void draw_debug_char(Yt::RenderPassImpl& pass, size_t x, size_t y, size_t width, size_t height, Yt::Bgra32 color, uint8_t value)
	{
		pass.draw_rect(
			{ { static_cast<float>(x * Yt::DebugTexture::char_width), static_cast<float>(y * Yt::DebugTexture::char_height) },
				Yt::SizeF{ static_cast<float>(width * Yt::DebugTexture::char_width), static_cast<float>(height * Yt::DebugTexture::char_height) } },
			{ { Yt::DebugTexture::coords[value][0][0], Yt::DebugTexture::coords[value][0][1] },
				Yt::Vector2{ Yt::DebugTexture::coords[value][1][0], Yt::DebugTexture::coords[value][1][1] } },
			color);
	}
}

namespace Yt
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

	void DebugRenderer::set_color(Bgra32 color)
	{
		_color = color;
	}
}
