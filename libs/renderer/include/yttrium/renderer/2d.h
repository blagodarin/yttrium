// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/image/color.h>
#include <yttrium/geometry/margins.h>

#include <memory>
#include <string_view>
#include <vector>

namespace Yt
{
	class Quad;
	class RectF;
	class RenderPass;
	class SizeF;
	class Texture2D;
	class Viewport;

	class Renderer2D
	{
	public:
		explicit Renderer2D(Viewport&);
		~Renderer2D() noexcept;

		void addQuad(const Quad&);
		size_t addBorderlessRect(const RectF&);
		void addRect(const RectF&);
		void draw(RenderPass&);
		void rewriteBorderlessRect(size_t id, const RectF&);
		void setColor(Bgra32);
		void setTexture(const std::shared_ptr<const Texture2D>&);
		void setTextureRect(const RectF&, const MarginsF& = {});
		SizeF viewportSize() const noexcept;

	private:
		const std::unique_ptr<struct Renderer2DData> _data;
	};
}
