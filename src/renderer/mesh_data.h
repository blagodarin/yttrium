#ifndef _src_renderer_mesh_data_h_
#define _src_renderer_mesh_data_h_

#include <yttrium/memory/buffer.h>

#include <vector>

namespace Yttrium
{
	enum class VA;

	class MeshData
	{
	public:
		std::vector<VA> _vertex_format;
		Buffer _vertex_data;
		std::vector<uint32_t> _indices;

		bool make_uint16_indices(Buffer&) const;
	};
}

#endif
