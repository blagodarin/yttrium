#ifndef _src_renderer_gl_wrappers_h_
#define _src_renderer_gl_wrappers_h_

#include "gl.h"

namespace Yttrium
{
	class Allocator;
	class StaticString;
	class String;

	class GlBufferHandle
	{
	public:

		GlBufferHandle(const GlApi&, GLenum target);
		GlBufferHandle(GlBufferHandle&&);
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

	class GlProgramHandle
	{
	public:

		GlProgramHandle(const GlApi&);
		GlProgramHandle(GlProgramHandle&&);
		~GlProgramHandle();

		void attach(GLuint) const;
		GLuint get() const { return _handle; }
		String info_log(Allocator&) const;
		bool link() const;

		GlProgramHandle(const GlProgramHandle&) = delete;
		GlProgramHandle& operator=(const GlProgramHandle&) = delete;
		GlProgramHandle& operator=(GlProgramHandle&&) = delete;

	private:
		const GlApi& _gl;
		GLuint _handle = 0;
	};

	class GlShaderHandle
	{
	public:

		GlShaderHandle(const GlApi&, GLenum type);
		GlShaderHandle(GlShaderHandle&&);
		~GlShaderHandle();

		bool compile(const StaticString&) const;
		GLuint get() const { return _handle; }
		String info_log(Allocator&) const;

		GlShaderHandle(const GlShaderHandle&) = delete;
		GlShaderHandle& operator=(const GlShaderHandle&) = delete;
		GlShaderHandle& operator=(GlShaderHandle&&) = delete;

	private:
		const GlApi& _gl;
		const GLenum _type;
		GLuint _handle = 0;
	};

	class GlTextureHandle
	{
	public:

		GlTextureHandle(const GlApi&, GLenum target);
		GlTextureHandle(GlTextureHandle&&);
		~GlTextureHandle();

		void bind() const;
		void generate_mipmaps() const;
		void set_anisotropy_enabled(bool) const;
		void set_data(GLint level, GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels) const;
		void set_parameter(GLenum, GLint) const;

		GlTextureHandle(const GlTextureHandle&) = delete;
		GlTextureHandle& operator=(const GlTextureHandle&) = delete;
		GlTextureHandle& operator=(GlTextureHandle&&) = delete;

	private:
		const GlApi& _gl;
		const GLenum _target;
		GLuint _handle = 0;
	};

	class GlVertexArrayHandle
	{
	public:

		GlVertexArrayHandle(const GlApi&);
		GlVertexArrayHandle(GlVertexArrayHandle&&);
		~GlVertexArrayHandle();

		GlVertexArrayHandle(const GlVertexArrayHandle&) = delete;
		GlVertexArrayHandle& operator=(const GlVertexArrayHandle&) = delete;
		GlVertexArrayHandle& operator=(GlVertexArrayHandle&&) = delete;

	private:
		const GlApi& _gl;
		GLuint _handle = 0;
	};
}

#endif
