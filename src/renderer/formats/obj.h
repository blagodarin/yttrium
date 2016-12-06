#ifndef _renderer_formats_obj_h_
#define _renderer_formats_obj_h_

namespace Yttrium
{
	class MeshData;
	class Reader;

	MeshData load_obj_mesh(Reader&&);
}

#endif
