#ifndef _examples_tetrium_graphics_h_
#define _examples_tetrium_graphics_h_

#include "logic.h"

#include <yttrium/renderer/texture.h>

#include <memory>

namespace Yttrium
{
	class RectF;
	class Renderer;
	class SizeF;
	class Texture2D;
}

using namespace Yttrium;

class TetriumGraphics
{
public:
	explicit TetriumGraphics(Renderer&);

	void draw_field(const RectF&, const Tetrium::Field&, const Tetrium::Figure&) const;
	void draw_next_figure(const RectF&, const Tetrium::Figure&) const;

private:
	void draw_block(const RectF&, const SizeF& block_size, float x, float y) const;
	void draw_field_blocks(const RectF&, const SizeF& block_size, const Tetrium::Field&) const;
	void draw_field_figure(const RectF&, const SizeF& block_size, const Tetrium::Figure&) const;
	void draw_field_frame(const RectF&, const SizeF& block_size) const;
	void set_texture_rect(Tetrium::Figure::Type) const;

private:
	Renderer& _renderer;
	const std::unique_ptr<const Texture2D> _blocks_texture;
};

#endif
