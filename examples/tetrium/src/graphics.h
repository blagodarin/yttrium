#ifndef _examples_tetrium_graphics_h_
#define _examples_tetrium_graphics_h_

#include "logic.h"

#include <yttrium/renderer/texture.h>

#include <memory>

namespace Yttrium
{
	class RectF;
	class Renderer;
	class RenderContext;
	class SizeF;
	class Texture2D;
}

using namespace Yttrium;

class TetriumGraphics
{
public:
	explicit TetriumGraphics(Renderer&);

	void draw_field(RenderContext&, const RectF&, const Tetrium::Field&, const Tetrium::Figure&) const;
	void draw_next_figure(RenderContext&, const RectF&, const Tetrium::Figure&) const;

private:
	void draw_block(RenderContext&, const RectF&, const SizeF& block_size, float x, float y) const;
	void draw_block(RenderContext& context, const RectF& rect, const SizeF& block_size, int x, int y) const { draw_block(context, rect, block_size, static_cast<float>(x), static_cast<float>(y)); }
	void draw_field_blocks(RenderContext&, const RectF&, const SizeF& block_size, const Tetrium::Field&) const;
	void draw_field_figure(RenderContext&, const RectF&, const SizeF& block_size, const Tetrium::Figure&) const;
	void draw_field_frame(RenderContext&, const RectF&, const SizeF& block_size) const;
	void set_texture_rect(RenderContext&, Tetrium::Figure::Type) const;

private:
	const std::unique_ptr<const Texture2D> _blocks_texture;
};

#endif
