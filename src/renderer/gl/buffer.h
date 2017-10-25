#ifndef _src_renderer_gl_buffer_h_
#define _src_renderer_gl_buffer_h_

#include "../buffer.h"
#include "wrappers.h"

namespace Yttrium
{
	struct GlIndexBuffer final : IndexBufferImpl
	{
		const GlBufferHandle _buffer;
		const GLenum _gl_format;

		GlIndexBuffer(IndexFormat, size_t count, size_t element_size, GlBufferHandle&& buffer, GLenum gl_format);

		void write(size_t offset, size_t count, const void* data) override;
	};

	struct GlVertexBuffer final : VertexBufferImpl
	{
		const GlBufferHandle _buffer;
		const GlVertexArrayHandle _vertex_array;

		GlVertexBuffer(size_t count, size_t element_size, GlBufferHandle&&, GlVertexArrayHandle&&);

		void write(size_t first, size_t count, const void* data) override;
	};
}

#endif
