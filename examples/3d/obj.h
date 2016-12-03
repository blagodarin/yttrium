#ifndef _examples_3d_obj_h_
#define _examples_3d_obj_h_

namespace Yttrium
{
	class IndexBuffer;
	class Reader;
	class Renderer;
	template <typename> class UniquePtr;
	class VertexBuffer;
}

using namespace Yttrium;

void load_obj_mesh(Reader&&, Renderer&, UniquePtr<VertexBuffer>&, UniquePtr<IndexBuffer>&);

#endif
