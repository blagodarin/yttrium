#ifndef _examples_tetrium_graphics_h_
#define _examples_tetrium_graphics_h_

#include "logic.h"

#include <yttrium/renderer/texture.h>

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

#endif
