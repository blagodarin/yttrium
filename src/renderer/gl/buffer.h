#ifndef _src_renderer_gl_buffer_h_
#define _src_renderer_gl_buffer_h_

#include "../buffer.h"
#include "wrappers.h"

namespace Yttrium
{
	class GlIndexBuffer : public IndexBufferImpl
	{
	public:

		const GlBufferHandle _buffer;
		const GLenum _gl_format;

		GlIndexBuffer(Format format, size_t size, size_t element_size, GlBufferHandle&& buffer, GLenum gl_format);

		void write(size_t offset, size_t size, const void* data) override;
	};

	class GlVertexBuffer : public VertexBufferImpl
	{
	public:

		const GlBufferHandle _buffer;

		GlVertexBuffer(std::initializer_list<VA>, size_t size, size_t element_size, GlBufferHandle&&, Allocator&);

		void write(size_t offset, size_t size, const void* data) override;
	};
}

#endif
