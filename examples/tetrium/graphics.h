//
// Copyright 2018 Sergei Blagodarin
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

#include <yttrium/renderer/texture.h>
#include "logic.h"

#include <memory>

namespace Yttrium
{
	class RectF;
	class RenderManager;
	class RenderPass;
	class SizeF;
	class Texture2D;
}

using namespace Yttrium;

class TetriumGraphics
{
public:
	explicit TetriumGraphics(RenderManager&);

	void draw_field(RenderPass&, const RectF&, const Tetrium::Field&, const Tetrium::Figure&) const;
	void draw_next_figure(RenderPass&, const RectF&, const Tetrium::Figure&) const;

private:
	void draw_block(RenderPass&, const RectF&, const SizeF& block_size, float x, float y) const;
	void draw_block(RenderPass& context, const RectF& rect, const SizeF& block_size, int x, int y) const { draw_block(context, rect, block_size, static_cast<float>(x), static_cast<float>(y)); }
	void draw_field_blocks(RenderPass&, const RectF&, const SizeF& block_size, const Tetrium::Field&) const;
	void draw_field_figure(RenderPass&, const RectF&, const SizeF& block_size, const Tetrium::Figure&) const;
	void draw_field_frame(RenderPass&, const RectF&, const SizeF& block_size) const;
	void set_texture_rect(RenderPass&, Tetrium::Figure::Type) const;

private:
	const std::unique_ptr<const Texture2D> _blocks_texture;
};
