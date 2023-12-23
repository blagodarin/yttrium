// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string_view>
#include <vector>

namespace seir
{
	class Line3;
	class Mat4;
	class RectF;
	class Vec2;
}

namespace Yt
{
	class Mesh;

	///
	class RenderPass
	{
	public:
		virtual ~RenderPass() noexcept = default;

		///
		virtual void draw_mesh(const Mesh&) = 0;

		///
		virtual seir::Mat4 full_matrix() const = 0;

		///
		virtual seir::Mat4 model_matrix() const = 0;

		///
		virtual seir::Line3 pixel_ray(const seir::Vec2&) const = 0;

		///
		virtual seir::RectF viewport_rect() const = 0;
	};
}
