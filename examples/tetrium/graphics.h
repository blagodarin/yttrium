#ifndef _examples_tetrium_graphics_h_
#define _examples_tetrium_graphics_h_

#include <yttrium/resources/resource_ptr.h>

#include "logic.h"

namespace Yttrium
{
	class RectF;
	class Renderer;
	class SizeF;
	class StaticString;
	class Texture2D;
}

using namespace Yttrium;

class TetriumGraphics
{
public:
	TetriumGraphics(Renderer&);

	void draw(const StaticString& canvas, const RectF&, const Tetrium::Game&) const;

private:
	void draw_block(const RectF&, const SizeF& block_size, float x, float y) const;
	void draw_field(const RectF&, const Tetrium::Field&, const Tetrium::Figure&) const;
	void draw_field_blocks(const RectF&, const SizeF& block_size, const Tetrium::Field&) const;
	void draw_field_figure(const RectF&, const SizeF& block_size, const Tetrium::Figure&) const;
	void draw_field_frame(const RectF&, const SizeF& block_size) const;
	void draw_next_figure(const RectF&, const Tetrium::Figure&) const;
	void set_texture_rect(Tetrium::Figure::Type) const;

private:
	Renderer& _renderer;
	const ResourcePtr<Texture2D> _blocks_texture;
};

#endif
