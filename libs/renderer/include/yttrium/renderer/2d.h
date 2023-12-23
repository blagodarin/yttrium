// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <seir_graphics/marginsf.hpp>

#include <memory>
#include <string_view>
#include <vector>

namespace seir
{
	class QuadF;
	class RectF;
	class Rgba32;
	class SizeF;
}

namespace Yt
{
	class RenderPass;
	class Texture2D;
	class Viewport;

	class Renderer2D
	{
	public:
		explicit Renderer2D(Viewport&);
		~Renderer2D() noexcept;

		void addQuad(const seir::QuadF&);
		size_t addBorderlessRect(const seir::RectF&);
		void addRect(const seir::RectF&);
		void draw(RenderPass&);
		void rewriteBorderlessRect(size_t id, const seir::RectF&);
		void setColor(const seir::Rgba32&);
		void setTexture(const std::shared_ptr<const Texture2D>&);
		void setTextureRect(const seir::RectF&, const seir::MarginsF& = {});
		seir::SizeF viewportSize() const noexcept;

	private:
		const std::unique_ptr<struct Renderer2DData> _data;
	};
}
