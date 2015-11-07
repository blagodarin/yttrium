#include "gpu_program.h"

#include <yttrium/log.h>
#include <yttrium/static_string.h>
#include "renderer.h"

#include <stdexcept>

namespace Yttrium
{
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

	String GlShaderHandle::info_log(Allocator& allocator) const
	{
		String result(&allocator);
		GLint info_log_length = 0;
		_gl.GetShaderiv(_handle, GL_INFO_LOG_LENGTH, &info_log_length);
		if (info_log_length > 0)
		{
			result.resize(info_log_length - 1);
			GLsizei length = 0;
			_gl.GetShaderInfoLog(_handle, info_log_length, &length, result.text());
			assert(static_cast<GLsizei>(result.size()) == length);
		}
		return result;
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

	String GlProgramHandle::info_log(Allocator& allocator) const
	{
		String result(&allocator);
		GLint info_log_length = 0;
		_gl.GetProgramiv(_handle, GL_INFO_LOG_LENGTH, &info_log_length);
		if (info_log_length > 0)
		{
			result.resize(info_log_length - 1);
			GLsizei length = 0;
			_gl.GetProgramInfoLog(_handle, info_log_length, &length, result.text());
			assert(static_cast<GLsizei>(result.size()) == length);
		}
		return result;
	}

	bool GlProgramHandle::link() const
	{
		_gl.LinkProgram(_handle);
		GLint link_status = GL_FALSE;
		_gl.GetProgramiv(_handle, GL_LINK_STATUS, &link_status);
		return GL_TRUE == link_status;
	}

	GlGpuProgram::GlGpuProgram(RendererImpl& renderer, GlShaderHandle&& vertex_shader, GlShaderHandle&& fragment_shader, const GlApi& gl)
		: _renderer(renderer)
		, _vertex_shader(std::move(vertex_shader))
		, _fragment_shader(std::move(fragment_shader))
		, _program(gl)
	{
		_program.attach(_vertex_shader.get());
		_program.attach(_fragment_shader.get());
	}

	GlGpuProgram::~GlGpuProgram()
	{
		_renderer.forget_program(this);
	}

	bool GlGpuProgram::link()
	{
		if (_program.link())
			return true;
#if Y_IS_DEBUG
		Log() << _program.info_log(_renderer.allocator());
#endif
		return false;
	}
}
