#ifndef _examples_tetrium_blocks_h_
#define _examples_tetrium_blocks_h_

#include <yttrium/renderer/texture.h>
#include <yttrium/resources/resource_ptr.h>

namespace Yttrium
{
	class PointF;
	class Renderer;
	class SizeF;
}

using namespace Yttrium;

class TetriumBlocks
{
public:
	TetriumBlocks(Renderer&);
	~TetriumBlocks();

	Texture2D* texture() { return _texture.get(); }

	static SizeF block_size();
	static PointF block_coords(size_t index);

private:
	ResourcePtr<Texture2D> _texture;
};

#endif
