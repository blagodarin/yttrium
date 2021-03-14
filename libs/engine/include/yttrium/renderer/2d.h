// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/math/color.h>
#include <yttrium/math/margins.h>

#include <memory>
#include <string_view>
#include <vector>

namespace Yt
{
	class Quad;
	class RectF;
	class RenderPass;
	class Texture2D;
	class Viewport;

	class Renderer2D
	{
	public:
		explicit Renderer2D(Viewport&);
		~Renderer2D() noexcept;

		void addQuad(const Quad&);
		void addRect(const RectF&);
		void draw(RenderPass&);
		void setColor(Bgra32);
		void setTexture(const std::shared_ptr<const Texture2D>&);
		void setTextureRect(const RectF&, const MarginsF& = {});

	private:
		const std::unique_ptr<struct Renderer2DData> _data;
	};
}
