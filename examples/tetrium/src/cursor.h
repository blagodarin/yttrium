#ifndef _examples_tetrium_cursor_h_
#define _examples_tetrium_cursor_h_

#include <yttrium/memory/pointer.h>

namespace Yttrium
{
	class IndexBuffer;
	class PointF;
	class Renderer;
	class VertexBuffer;
}

using namespace Yttrium;

class Cursor
{
public:
	Cursor(Renderer&);
	~Cursor();

	void draw(const PointF&);

protected:
	Renderer& _renderer;
	const Pointer<VertexBuffer> _vertices;
	const Pointer<IndexBuffer> _indices;
};

#endif
