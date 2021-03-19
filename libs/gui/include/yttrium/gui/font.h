// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/math/rect.h>

#include <memory>
#include <string_view>
#include <vector>

namespace Yt
{
	class RenderManager;
	class Renderer2D;
	class Source;
	class Texture2D;

	class Font
	{
	public:
		struct TextCapture
		{
			const size_t _cursor_pos;
			bool _has_cursor = false;
			RectF _cursor_rect;

			const size_t _selection_begin;
			const size_t _selection_end;
			bool _has_selection = false;
			RectF _selection_rect;

			constexpr TextCapture(size_t cursor_pos, size_t selection_begin, size_t selection_size) noexcept
				: _cursor_pos{ cursor_pos }, _selection_begin{ selection_begin }, _selection_end{ _selection_begin + selection_size } {}
		};

		static std::shared_ptr<const Font> load(const Source&, RenderManager&);

		virtual ~Font() noexcept = default;
		virtual void render(Renderer2D&, const RectF&, std::string_view, TextCapture* = nullptr) const = 0;
		virtual SizeF text_size(std::string_view, float font_size) const = 0;
		virtual std::shared_ptr<const Texture2D> texture() const noexcept = 0;
		virtual RectF white_rect() const noexcept = 0;
	};
}
