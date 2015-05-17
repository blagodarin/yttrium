#include "buffer.h"

#include <utility> // std::move

namespace Yttrium
{
	GLBufferHandle::GLBufferHandle(const GlApi& gl, GLenum target)
		: _gl(gl)
		, _target(target)
	{
		_gl.GenBuffers(1, &_handle);
		// TODO: Throw some nasty exception if it fails.
	}

	GLBufferHandle::GLBufferHandle(GLBufferHandle&& handle)
		: _gl(handle._gl)
		, _target(handle._target)
		, _handle(handle._handle)
	{
		handle._handle = 0;
	}

	GLBufferHandle::~GLBufferHandle()
	{
		if (_handle != 0)
			_gl.DeleteBuffers(1, &_handle);
	}

	void GLBufferHandle::bind() const
	{
		_gl.BindBuffer(_target, _handle);
	}

	void GLBufferHandle::initialize(GLenum usage, size_t size, const void* data) const
	{
		_gl.BufferData(_target, size, data, usage);
	}

	void GLBufferHandle::unbind() const
	{
		_gl.BindBuffer(_target, 0);
	}

	void GLBufferHandle::write(size_t offset, size_t size, const void* data) const
	{
		_gl.BufferSubData(_target, offset, size, data);
	}

	GLIndexBuffer::GLIndexBuffer(Format format, size_t size, size_t element_size, GLBufferHandle&& buffer, GLenum gl_format)
		: IndexBufferImpl(format, size, element_size)
		, _buffer(std::move(buffer))
		, _gl_format(gl_format)
	{
	}

	void GLIndexBuffer::write(size_t offset, size_t size, const void* data)
	{
		_buffer.bind();
		_buffer.write(_element_size * offset, _element_size * size, data);
		_buffer.unbind();
	}

	GLVertexBuffer::GLVertexBuffer(unsigned format, size_t size, size_t element_size, GLBufferHandle&& buffer)
		: VertexBufferImpl(format, size, element_size)
		, _buffer(std::move(buffer))
	{
	}

	void GLVertexBuffer::write(size_t offset, size_t size, const void* data)
	{
		_buffer.bind();
		_buffer.write(_element_size * offset, _element_size * size, data);
		_buffer.unbind();
	}
}
