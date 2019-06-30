//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include <yttrium/math/color.h>

#include <string_view>
#include <vector>

namespace Yttrium
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
		virtual void add_debug_text(std::string_view) = 0;

		///
		virtual void draw_mesh(const Mesh&) = 0;

		///
		virtual void draw_quad(const Quad&, const Color4f& = { 1, 1, 1 }) = 0;

		///
		virtual void draw_rect(const RectF&, const Color4f& = { 1, 1, 1 }) = 0;

		///
		virtual void draw_rects(const std::vector<TexturedRect>&, const Color4f& = { 1, 1, 1 }) = 0;

		///
		virtual Matrix4 full_matrix() const = 0;

		///
		virtual Matrix4 model_matrix() const = 0;

		///
		virtual Line3 pixel_ray(const Vector2&) const = 0;

		///
		virtual void set_texture_rect(const RectF&, const MarginsF&) = 0;

		///
		virtual SizeF window_size() const = 0;
	};
}
