#include "buffer.h"

#include <utility>

namespace Yttrium
{
	GlIndexBuffer::GlIndexBuffer(Format format, size_t size, size_t element_size, GlBufferHandle&& buffer, GLenum gl_format)
		: IndexBufferImpl(format, size, element_size)
		, _buffer(std::move(buffer))
		, _gl_format(gl_format)
	{
	}

	void GlIndexBuffer::write(size_t offset, size_t size, const void* data)
	{
		_buffer.write(_element_size * offset, _element_size * size, data);
	}

	GlVertexBuffer::GlVertexBuffer(unsigned format, size_t size, size_t element_size, GlBufferHandle&& buffer)
		: VertexBufferImpl(format, size, element_size)
		, _buffer(std::move(buffer))
	{
	}

	void GlVertexBuffer::write(size_t offset, size_t size, const void* data)
	{
		_buffer.write(_element_size * offset, _element_size * size, data);
	}
}
