#ifndef _examples_tetrium_blocks_h_
#define _examples_tetrium_blocks_h_

#include <yttrium/math/point.h>
#include <yttrium/math/size.h>
#include <yttrium/memory/unique_ptr.h>

namespace Yttrium
{
	class Renderer;
	class Texture2D;
}

using namespace Yttrium;

class Blocks
{
public:
	Blocks(Renderer&);
	~Blocks();

	Texture2D* texture() { return _texture.get(); }

	static SizeF block_size();
	static PointF block_coords(size_t index);

protected:
	UniquePtr<Texture2D> _texture;
};

#endif
