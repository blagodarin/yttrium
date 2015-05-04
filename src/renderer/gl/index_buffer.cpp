#include "index_buffer.h"

#include <utility> // std::move

namespace Yttrium
{
	GlIndexBuffer::GlIndexBuffer(Format format, size_t size, size_t element_size, GLBufferHandle&& buffer, GLenum gl_format)
		: IndexBufferImpl(format, size, element_size)
		, _buffer(std::move(buffer))
		, _gl_format(gl_format)
	{
	}

	void GlIndexBuffer::write(size_t offset, size_t size, const void* data)
	{
		_buffer.bind();
		_buffer.write(_element_size * offset, _element_size * size, data);
		_buffer.unbind();
	}
}
