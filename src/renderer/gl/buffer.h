#ifndef _src_renderer_gl_buffer_h_
#define _src_renderer_gl_buffer_h_

#include "../buffer.h"
#include "gl.h"

namespace Yttrium
{
	class GlBufferHandle
	{
	public:

		GlBufferHandle(const GlApi& gl, GLenum target);
		GlBufferHandle(GlBufferHandle&& handle);
		~GlBufferHandle();

		void bind() const;
		void initialize(GLenum usage, size_t size, const void* data) const;
		void unbind() const;
		void write(size_t offset, size_t size, const void* data) const;

		GlBufferHandle(const GlBufferHandle&) = delete;
		GlBufferHandle& operator=(const GlBufferHandle&) = delete;
		GlBufferHandle& operator=(GlBufferHandle&&) = delete;

	private:

		const GlApi& _gl;
		const GLenum _target;
		GLuint _handle = 0;
	};

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

		GlVertexBuffer(unsigned format, size_t size, size_t element_size, GlBufferHandle&& buffer);

		void write(size_t offset, size_t size, const void* data) override;
	};
}

#endif
