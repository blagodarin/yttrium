#include "index_buffer.h"

namespace Yttrium
{
	GlIndexBuffer::GlIndexBuffer(Format format, size_t size, const GlApi& gl, GLuint buffer)
		: IndexBufferImpl(format, size)
		, _gl(gl)
		, _buffer(buffer)
	{
	}

	GlIndexBuffer::~GlIndexBuffer()
	{
		_gl.DeleteBuffersARB(1, &_buffer);
	}

	void GlIndexBuffer::write(const void* data, size_t size, size_t offset)
	{
		_gl.BindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _buffer);
		_gl.BufferSubDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, offset * element_size(), size * element_size(), data);
	}
}
