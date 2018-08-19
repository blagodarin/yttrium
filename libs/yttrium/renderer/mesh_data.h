#ifndef _src_renderer_mesh_data_h_
#define _src_renderer_mesh_data_h_

#include <yttrium/memory/buffer.h>

#include <vector>

namespace Yttrium
{
	/// Vertex attribute type.
	enum class VA
	{
		f,  ///< Single float.
		f2, ///< Vector of 2 floats.
		f3, ///< Vector of 3 floats.
		f4, ///< Vector of 4 floats.
	};

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
