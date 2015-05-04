#ifndef __RENDERER_GL_INDEX_BUFFER_H
#define __RENDERER_GL_INDEX_BUFFER_H

#include "../index_buffer.h"
#include "buffer.h"

namespace Yttrium
{
	class GLBufferHandle;

	class GlIndexBuffer: public IndexBufferImpl
	{
	public:

		const GLBufferHandle _buffer;
		const GLenum _gl_format;

		GlIndexBuffer(Format format, size_t size, size_t element_size, GLBufferHandle&& buffer, GLenum gl_format);

		void write(size_t offset, size_t size, const void* data) override;
	};
}

#endif
