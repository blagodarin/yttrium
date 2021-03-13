// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/math/color.h>

#include <string_view>
#include <vector>

namespace Yt
{
	class Line3;
	class MarginsF;
	class Matrix4;
	class Mesh;
	class Quad;
	class RectF;
	class SizeF;
	class TexturedRect;
	class Vector2;

	///
	class RenderPass
	{
	public:
		virtual ~RenderPass() noexcept = default;

		///
		virtual void draw_mesh(const Mesh&) = 0;

		///
		virtual Matrix4 full_matrix() const = 0;

		///
		virtual Matrix4 model_matrix() const = 0;

		///
		virtual Line3 pixel_ray(const Vector2&) const = 0;

		///
		virtual SizeF window_size() const = 0;
	};
}
