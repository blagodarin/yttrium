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
			Rgba4F = 1 << 0, ///< Followed by color data (R, G, B, A floats).
			Uv2F   = 1 << 1, ///< Followed by 2D texture coordinates (U, V floats).
		};

		virtual ~VertexBuffer() = default;

		/// Returns the vertex format flags.
		virtual unsigned format() const = 0;

		/// Returns the number of elements in the buffer.
		virtual size_t size() const = 0;

		/// Updates the buffer contents.
		virtual void write(const void* data, size_t size, size_t offset) = 0;
	};
}

#endif // __Y_VERTEX_BUFFER_H
