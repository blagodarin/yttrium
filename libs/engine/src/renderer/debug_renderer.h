// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/math/color.h>
#include <yttrium/renderer/modifiers.h>

#include <optional>
#include <string_view>

namespace Yt
{
	class RenderPassImpl;

	class DebugRenderer
	{
	public:
		explicit DebugRenderer(RenderPass&);

		void draw_cursor(size_t x, size_t y);
		void draw_rectangle(size_t x, size_t y, size_t width, size_t height);
		void draw_text(size_t x, size_t y, std::string_view text, const std::optional<size_t>& max_size = {});
		size_t max_width() const;
		void set_color(Bgra32);

	private:
		RenderPassImpl& _pass;
		PushTexture _texture;
		Bgra32 _color = Bgra32::white();
	};
}
