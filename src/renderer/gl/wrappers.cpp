#include "wrappers.h"

#include <yttrium/math/matrix.h>
#include <yttrium/string.h>

#include <cassert>
#include <stdexcept>

namespace Yttrium
{
	GlBufferHandle::GlBufferHandle(const GlApi& gl, GLenum target)
		: _gl(gl)
		, _target(target)
	{
		_gl.GenBuffers(1, &_handle);
		if (!_handle)
			throw std::runtime_error("glGenBuffers failed");
	}

	GlBufferHandle::GlBufferHandle(GlBufferHandle&& buffer)
		: _gl(buffer._gl)
		, _target(buffer._target)
		, _handle(buffer._handle)
		, _size(buffer._size)
	{
		buffer._handle = 0;
	}

	GlBufferHandle::~GlBufferHandle()
	{
		if (_handle)
			_gl.DeleteBuffers(1, &_handle);
	}

	void GlBufferHandle::bind() const
	{
		_gl.BindBuffer(_target, _handle);
	}

	void GlBufferHandle::initialize(GLenum usage, size_t size, const void* data)
	{
		_gl.NamedBufferDataEXT(_handle, size, data, usage);
		_size = size;
	}

	void GlBufferHandle::unbind() const
	{
		_gl.BindBuffer(_target, 0);
	}

	void GlBufferHandle::write(size_t offset, size_t size, const void* data) const
	{
		_gl.NamedBufferSubDataEXT(_handle, offset, size, data);
	}

	GlProgramHandle::GlProgramHandle(const GlApi& gl)
		: _gl(gl)
		, _handle(_gl.CreateProgram())
	{
		if (!_handle)
			throw std::runtime_error("glCreateProgram failed");
	}

	GlProgramHandle::GlProgramHandle(GlProgramHandle&& program)
		: _gl(program._gl)
		, _handle(program._handle)
	{
		program._handle = 0;
	}

	GlProgramHandle::~GlProgramHandle()
	{
		if (_handle)
			_gl.DeleteProgram(_handle);
	}

	void GlProgramHandle::attach(GLuint shader) const
	{
		_gl.AttachShader(_handle, shader);
	}

	std::string GlProgramHandle::info_log() const
	{
		GLint info_log_length = 0;
		_gl.GetProgramiv(_handle, GL_INFO_LOG_LENGTH, &info_log_length);
		if (info_log_length <= 1)
			return {};
		std::string result(info_log_length - 1, ' ');
		GLsizei length = 0;
		_gl.GetProgramInfoLog(_handle, info_log_length, &length, &result[0]);
		assert(length == info_log_length - 1);
		return result;
	}

	bool GlProgramHandle::link() const
	{
		_gl.LinkProgram(_handle);
		GLint link_status = GL_FALSE;
		_gl.GetProgramiv(_handle, GL_LINK_STATUS, &link_status);
		return GL_TRUE == link_status;
	}

	void GlProgramHandle::set_uniform(const char* name, const Matrix4& value) const
	{
		_gl.ProgramUniformMatrix4fvEXT(_handle, _gl.GetUniformLocation(_handle, name), 1, GL_FALSE, value.data());
	}

	GlShaderHandle::GlShaderHandle(const GlApi& gl, GLenum type)
		: _gl(gl)
		, _type(type)
		, _handle(_gl.CreateShader(_type))
	{
		if (!_handle)
			throw std::runtime_error("glCreateShader failed");
	}

	GlShaderHandle::GlShaderHandle(GlShaderHandle&& shader)
		: _gl(shader._gl)
		, _type(shader._type)
		, _handle(shader._handle)
	{
		shader._handle = 0;
	}

	GlShaderHandle::~GlShaderHandle()
	{
		if (_handle)
			_gl.DeleteShader(_handle);
	}

	bool GlShaderHandle::compile(const StaticString& source) const
	{
		const GLchar* source_text = source.text();
		const GLint source_size = source.size();
		_gl.ShaderSource(_handle, 1, &source_text, &source_size);
		_gl.CompileShader(_handle);
		GLint compile_status = GL_FALSE;
		_gl.GetShaderiv(_handle, GL_COMPILE_STATUS, &compile_status);
		return GL_TRUE == compile_status;
	}

	std::string GlShaderHandle::info_log() const
	{
		GLint info_log_length = 0;
		_gl.GetShaderiv(_handle, GL_INFO_LOG_LENGTH, &info_log_length);
		if (info_log_length <= 1)
			return {};
		std::string result(info_log_length - 1, ' ');
		GLsizei length = 0;
		_gl.GetShaderInfoLog(_handle, info_log_length, &length, &result[0]);
		assert(length == info_log_length - 1);
		return result;
	}

	GlTextureHandle::GlTextureHandle(const GlApi& gl, GLenum target)
		: _gl(gl)
		, _target(target)
	{
		_gl.GenTextures(1, &_handle);
		if (!_handle)
			throw std::runtime_error("glGenTextures failed");
	}

	GlTextureHandle::GlTextureHandle(GlTextureHandle&& texture)
		: _gl(texture._gl)
		, _target(texture._target)
		, _handle(texture._handle)
	{
		texture._handle = 0;
	}

	GlTextureHandle::~GlTextureHandle()
	{
		if (_handle)
			_gl.DeleteTextures(1, &_handle);
	}

	void GlTextureHandle::bind() const
	{
		_gl.BindTexture(_target, _handle);
	}

	void GlTextureHandle::generate_mipmaps() const
	{
		_gl.GenerateTextureMipmapEXT(_handle, _target);
	}

	void GlTextureHandle::set_anisotropy_enabled(bool enabled) const
	{
		if (_gl.EXT_texture_filter_anisotropic)
			_gl.TextureParameterfEXT(_handle, _target, GL_TEXTURE_MAX_ANISOTROPY_EXT, enabled ? _gl.MAX_TEXTURE_MAX_ANISOTROPY_EXT : 1.f);
	}

	void GlTextureHandle::set_data(GLint level, GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels) const
	{
		_gl.TextureImage2DEXT(_handle, _target, level, internalformat, width, height, 0, format, type, pixels);
	}

	void GlTextureHandle::set_parameter(GLenum name, GLint value) const
	{
		_gl.TextureParameteriEXT(_handle, _target, name, value);
	}

	GlVertexArrayHandle::GlVertexArrayHandle(const GlApi& gl)
		: _gl(gl)
	{
		_gl.GenVertexArrays(1, &_handle);
		if (!_handle)
			throw std::runtime_error("glGenVertexArrays failed");
	}

	GlVertexArrayHandle::GlVertexArrayHandle(GlVertexArrayHandle&& vertex_array)
		: _gl(vertex_array._gl)
		, _handle(vertex_array._handle)
		, _attributes(vertex_array._attributes)
	{
		vertex_array._handle = 0;
	}

	GlVertexArrayHandle::~GlVertexArrayHandle()
	{
		if (_handle)
			_gl.DeleteVertexArrays(1, &_handle);
	}

	void GlVertexArrayHandle::bind() const
	{
		_gl.BindVertexArray(_handle);
		for (int i = 0; i < 32; ++i)
			if (_attributes & (1 << i))
				_gl.EnableVertexAttribArray(i);
	}

	void GlVertexArrayHandle::bind_vertex_buffer(GLuint binding, GLuint buffer, GLintptr offset, GLintptr stride)
	{
		_gl.VertexArrayBindVertexBufferEXT(_handle, binding, buffer, offset, stride);
	}

	void GlVertexArrayHandle::unbind() const
	{
		for (int i = 31; i >= 0; --i)
			if (_attributes & (1 << i))
				_gl.DisableVertexAttribArray(i);
		_gl.BindVertexArray(0);
	}

	void GlVertexArrayHandle::vertex_attrib_binding(GLuint attrib, GLuint binding)
	{
		_gl.VertexArrayVertexAttribBindingEXT(_handle, attrib, binding);
	}

	void GlVertexArrayHandle::vertex_attrib_format(GLuint attrib, GLint size, GLenum type, GLboolean normalized, GLuint offset)
	{
		_gl.VertexArrayVertexAttribFormatEXT(_handle, attrib, size, type, normalized, offset);
		assert(attrib < 32);
		_attributes |= 1 << attrib;
	}
}
