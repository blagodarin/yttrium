#ifndef __RENDERER_GL_BUFFER_H
#define __RENDERER_GL_BUFFER_H

#include "gl.h"

namespace Yttrium
{
	class GLBufferHandle
	{
	public:

		GLBufferHandle(const GlApi& gl, GLenum target)
			: _gl(gl)
			, _target(target)
		{
			_gl.GenBuffersARB(1, &_handle);
			// TODO: Throw some nasty exception if it fails.
		}

		GLBufferHandle(GLBufferHandle&& handle)
			: _gl(handle._gl)
			, _handle(handle._handle)
		{
			handle._handle = 0;
		}

		~GLBufferHandle()
		{
			if (_handle != 0)
				_gl.DeleteBuffersARB(1, &_handle);
		}

		explicit operator bool() const
		{
			return _handle != 0;
		}

		void bind() const
		{
			_gl.BindBufferARB(_target, _handle);
		}

		void initialize(GLenum usage, size_t size, const void* data) const
		{
			_gl.BufferDataARB(_target, size, data, usage);
		}

		void write(size_t offset, size_t size, const void* data) const
		{
			_gl.BufferSubDataARB(_target, offset, size, data);
		}

		GLBufferHandle(const GLBufferHandle&) = delete;
		GLBufferHandle& operator=(const GLBufferHandle&) = delete;
		GLBufferHandle& operator=(GLBufferHandle&&) = delete;

	private:

		const GlApi& _gl;
		GLenum _target = 0;
		GLuint _handle = 0;
	};
}

#endif
