#ifndef _examples_tetrium_graphics_h_
#define _examples_tetrium_graphics_h_

#include "blocks.h"
#include "logic.h"

namespace Yttrium
{
	class RectF;
}

using namespace Yttrium;

class TetriumGraphics
{
public:
	TetriumGraphics(Renderer& renderer) : _renderer(renderer) {}

	void draw_field(const RectF&, const Tetrium::Field&, const Tetrium::Figure&);
	void draw_next_figure(const RectF&, const Tetrium::Figure&);

private:
	void draw_field_blocks(const RectF&, const SizeF& block_size, const Tetrium::Field&);
	void draw_field_figure(const RectF&, const SizeF& block_size, const Tetrium::Figure&);
	void draw_field_frame(const RectF&, const SizeF& block_size);
	void set_texture_rect(Tetrium::Figure::Type);

private:
	Renderer& _renderer;
	TetriumBlocks _blocks{ _renderer };
};

#endif
