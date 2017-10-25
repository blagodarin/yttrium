#include "buffer.h"

#include <utility>

namespace Yttrium
{
	GlIndexBuffer::GlIndexBuffer(IndexFormat format, size_t count, size_t element_size, GlBufferHandle&& buffer, GLenum gl_format)
		: IndexBufferImpl{format, count, element_size}
		, _buffer{std::move(buffer)}
		, _gl_format{gl_format}
	{
	}

	void GlIndexBuffer::write(size_t offset, size_t count, const void* data)
	{
		_buffer.write(_element_size * offset, _element_size * count, data);
	}

	GlVertexBuffer::GlVertexBuffer(size_t count, size_t element_size, GlBufferHandle&& buffer, GlVertexArrayHandle&& vertex_array)
		: VertexBufferImpl{count, element_size}
		, _buffer{std::move(buffer)}
		, _vertex_array{std::move(vertex_array)}
	{
	}

	void GlVertexBuffer::write(size_t first, size_t count, const void* data)
	{
		_buffer.write(_element_size * first, _element_size * count, data);
	}
}
