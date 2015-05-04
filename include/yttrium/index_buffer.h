/// \file
/// \brief Index buffers.

#ifndef __Y_INDEX_BUFFER_H
#define __Y_INDEX_BUFFER_H

#include <cstddef> // size_t

namespace Yttrium
{
	/// Index buffer.
	class IndexBuffer
	{
	public:

		/// Index format.
		enum class Format
		{
			U16, ///< Unsigned 16-bit indices.
			U32, ///< Unsigned 32-bit indices.
		};

		virtual ~IndexBuffer() = default;

		/// Return the index format.
		virtual Format format() const = 0;

		/// Return the number of elements in the buffer.
		virtual size_t size() const = 0;

		/// Update the buffer contents.
		virtual void write(size_t offset, size_t size, const void* data) = 0;
	};
}

#endif // __Y_INDEX_BUFFER_H
