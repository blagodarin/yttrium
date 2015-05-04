#include "vertex_buffer.h"

#include <utility> // std::move

namespace Yttrium
{
	GlVertexBuffer::GlVertexBuffer(unsigned format, size_t size, size_t element_size, GLBufferHandle&& buffer)
		: VertexBufferImpl(format, size, element_size)
		, _buffer(std::move(buffer))
	{
	}

	void GlVertexBuffer::write(size_t offset, size_t size, const void* data)
	{
		_buffer.bind();
		_buffer.write(_element_size * offset, _element_size * size, data);
		_buffer.unbind();
	}
}
