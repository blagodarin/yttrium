#ifndef __RENDERER_GL_VERTEX_BUFFER_H
#define __RENDERER_GL_VERTEX_BUFFER_H

#include "../vertex_buffer.h"
#include "buffer.h"

namespace Yttrium
{
	class GLBufferHandle;

	class GlVertexBuffer: public VertexBufferImpl
	{
	public:

		const GLBufferHandle _buffer;

		GlVertexBuffer(unsigned format, size_t size, size_t element_size, GLBufferHandle&& buffer);

		void write(size_t offset, size_t size, const void* data) override;
	};
}

#endif
