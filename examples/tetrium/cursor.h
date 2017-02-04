#ifndef _examples_tetrium_cursor_h_
#define _examples_tetrium_cursor_h_

#include <yttrium/memory/unique_ptr.h>

namespace Yttrium
{
	class IndexBuffer;
	class PointF;
	class Renderer;
	class VertexBuffer;
}

using namespace Yttrium;

class TetriumCursor
{
public:
	TetriumCursor(Renderer&);
	~TetriumCursor();

	void draw(const PointF&);

protected:
	Renderer& _renderer;
	const UniquePtr<VertexBuffer> _vertices;
	const UniquePtr<IndexBuffer> _indices;
};

#endif
