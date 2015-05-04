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

		GlVertexBuffer(unsigned format, size_t size, size_t element_size, GLBufferHandle&& buffer);

		void write(const void* data, size_t size, size_t offset) override;

	private:

		const GLBufferHandle _buffer;
	};
}

#endif
