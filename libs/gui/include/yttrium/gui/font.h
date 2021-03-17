// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string_view>
#include <vector>

namespace Yt
{
	class RectF;
	class RenderManager;
	class Renderer2D;
	class Size;
	class SizeF;
	class Source;
	class TextCapture;
	class Texture2D;
	class Vector2;

	class Font
	{
	public:
		static std::shared_ptr<const Font> load(const Source&, RenderManager&);

		virtual ~Font() noexcept = default;
		virtual void render(Renderer2D&, const Vector2& topLeft, float fontSize, std::string_view, TextCapture* = nullptr) const = 0;
		virtual Size text_size(std::string_view) const = 0;
		virtual SizeF text_size(std::string_view, const SizeF& font_size) const = 0;
		virtual std::shared_ptr<const Texture2D> texture() const noexcept = 0;
		virtual RectF white_rect() const noexcept = 0;
	};
}
