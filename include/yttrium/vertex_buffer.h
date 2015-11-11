/// \file
/// \brief Vertex buffer.

#ifndef _include_yttrium_vertex_buffer_h_
#define _include_yttrium_vertex_buffer_h_

#include <cstddef>

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

	/// Vertex buffer.
	class VertexBuffer
	{
	public:

		virtual ~VertexBuffer() = default;

		/// Returns the number of vertices in the buffer.
		virtual size_t size() const = 0;

		/// Updates the buffer contents.
		virtual void write(size_t offset, size_t size, const void* data) = 0;
	};
}

#endif
