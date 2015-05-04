#ifndef __RENDERER_GL_BUFFER_H
#define __RENDERER_GL_BUFFER_H

#include "../buffer.h"
#include "gl.h"

namespace Yttrium
{
	class GLBufferHandle
	{
	public:

		GLBufferHandle(const GlApi& gl, GLenum target);
		GLBufferHandle(GLBufferHandle&& handle);
		~GLBufferHandle();

		explicit operator bool() const { return _handle != 0; }

		void bind() const;
		void initialize(GLenum usage, size_t size, const void* data) const;
		void unbind() const;
		void write(size_t offset, size_t size, const void* data) const;

		GLBufferHandle(const GLBufferHandle&) = delete;
		GLBufferHandle& operator=(const GLBufferHandle&) = delete;
		GLBufferHandle& operator=(GLBufferHandle&&) = delete;

	private:

		const GlApi& _gl;
		const GLenum _target;
		GLuint _handle = 0;
	};

	class GLIndexBuffer: public IndexBufferImpl
	{
	public:

		const GLBufferHandle _buffer;
		const GLenum _gl_format;

		GLIndexBuffer(Format format, size_t size, size_t element_size, GLBufferHandle&& buffer, GLenum gl_format);

		void write(size_t offset, size_t size, const void* data) override;
	};

	class GLVertexBuffer: public VertexBufferImpl
	{
	public:

		const GLBufferHandle _buffer;

		GLVertexBuffer(unsigned format, size_t size, size_t element_size, GLBufferHandle&& buffer);

		void write(size_t offset, size_t size, const void* data) override;
	};
}

#endif
