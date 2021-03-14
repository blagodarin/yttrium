// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/math/color.h>

#include <memory>
#include <string_view>
#include <vector>

namespace Yt
{
	class RenderManager;
	class Renderer2D;
	class Size;
	class SizeF;
	class Source;
	class TextCapture;
	class Vector2;

	class Font
	{
	public:
		static std::shared_ptr<const Font> load(const Source&, RenderManager&);

		virtual ~Font() noexcept = default;
		virtual void render(Renderer2D&, Bgra32, const Vector2& topLeft, float fontSize, std::string_view, TextCapture* = nullptr) const = 0;
		virtual Size text_size(std::string_view) const = 0;
		virtual SizeF text_size(std::string_view, const SizeF& font_size) const = 0;
	};
}
