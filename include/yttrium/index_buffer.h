/// \file
/// \brief Index buffer.

#ifndef _include_yttrium_index_buffer_h_
#define _include_yttrium_index_buffer_h_

#include <cstddef>

namespace Yttrium
{
	/// Index format.
	enum class IndexFormat
	{
		U16, ///< Unsigned 16-bit indices.
		U32, ///< Unsigned 32-bit indices.
	};

	/// Index buffer.
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		/// Returns the index format.
		virtual IndexFormat format() const = 0;

		/// Returns the number of indices in the buffer.
		virtual size_t size() const = 0;

		/// Updates the buffer contents.
		virtual void write(size_t offset, size_t size, const void* data) = 0;
	};
}

#endif
