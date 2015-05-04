#ifndef __RENDERER_GL_INDEX_BUFFER_H
#define __RENDERER_GL_INDEX_BUFFER_H

#include "../index_buffer.h"
#include "gl.h"

namespace Yttrium
{
	class GlIndexBuffer: public IndexBufferImpl
	{
	public:

		GlIndexBuffer(Format format, size_t size, const GlApi& gl, GLuint buffer);
		~GlIndexBuffer() override;

		void write(const void* data, size_t size, size_t offset) override;

	private:

		const GlApi& _gl;
		GLuint _buffer = 0;
	};
}

#endif
