/// \file
/// \brief Vertex buffers.

#ifndef __Y_VERTEX_BUFFER_H
#define __Y_VERTEX_BUFFER_H

#include <cstddef> // size_t

namespace Yttrium
{
	/// Vertex buffer.
	class VertexBuffer
	{
	public:

		/// Vertex format flags.
		/// A buffer vertex always start with geometry data (X, Y, Z, W floats).
		enum
		{
			Rgba4F   = 1 << 0, ///< Followed by color data (R, G, B, A floats).
			Uv2F     = 1 << 1, ///< Followed by 2D texture coordinates (U, V floats).
			Normal4F = 1 << 2, ///< Followed by 3D normal vector (X, Y, Z, W floats, W is ignored).
		};

		virtual ~VertexBuffer() = default;

		/// Return the vertex format flags.
		virtual unsigned format() const = 0;

		/// Return the number of elements in the buffer.
		virtual size_t size() const = 0;

		/// Update the buffer contents.
		virtual void write(size_t offset, size_t size, const void* data) = 0;
	};
}

#endif // __Y_VERTEX_BUFFER_H
