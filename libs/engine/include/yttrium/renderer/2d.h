// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/math/color.h>
#include <yttrium/math/margins.h>
#include <yttrium/math/vector.h>

#include <memory>
#include <string_view>
#include <vector>

namespace Yt
{
	class Quad;
	class RectF;
	class RenderPass;
	class Texture2D;
	class TexturedRect;
	class Viewport;

	struct Vertex2D
	{
		Vector2 _position;
		Vector2 _texture;
		Bgra32 _color;
	};

	class Renderer2D
	{
	public:
		explicit Renderer2D(Viewport&);
		~Renderer2D() noexcept;

		void addDebugText(std::string_view);
		void addQuad(const Quad&, Bgra32 = Bgra32::white());
		void addRect(const RectF&, Bgra32 = Bgra32::white());
		void addRects(const std::vector<TexturedRect>&, Bgra32 = Bgra32::white());
		void draw(RenderPass&);
		void setTexture(const std::shared_ptr<const Texture2D>&);
		void setTextureRect(const RectF&, const MarginsF& = {});

	private:
		const std::unique_ptr<struct Renderer2DData> _data;
	};
}
