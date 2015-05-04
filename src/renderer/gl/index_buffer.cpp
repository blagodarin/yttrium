#include "index_buffer.h"

#include <utility> // std::move

namespace Yttrium
{
	GlIndexBuffer::GlIndexBuffer(Format format, size_t size, size_t element_size, GLBufferHandle&& buffer)
		: IndexBufferImpl(format, size, element_size)
		, _buffer(std::move(buffer))
	{
	}

	void GlIndexBuffer::write(const void* data, size_t size, size_t offset)
	{
		_buffer.bind();
		_buffer.write(_element_size * offset, _element_size * size, data);
	}
}
