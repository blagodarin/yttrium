#include "gpu_program.h"

#include <yttrium/static_string.h>
#include "renderer.h"

#include <cassert>

#if Y_IS_DEBUG
	#include <yttrium/log.h>
#endif

namespace Yttrium
{
	GlGpuProgram::GlGpuProgram(RendererImpl& renderer, const GlApi& gl)
		: _renderer(renderer)
		, _gl(gl)
		, _program(_gl.CreateProgram())
	{
		assert(_program); // TODO: Throw.
	}

	GlGpuProgram::~GlGpuProgram()
	{
		_gl.DeleteProgram(_program);
		if (_fragment_shader)
			_gl.DeleteShader(_fragment_shader);
		if (_vertex_shader)
			_gl.DeleteShader(_vertex_shader);
		_renderer.forget_program(this);
	}

	bool GlGpuProgram::link()
	{
		if (_linked)
			return true;

		if (!_vertex_shader || !_fragment_shader)
			return false; // TODO: Throw?

		// TODO: Exclude the possibility of multiple linking.
		_gl.LinkProgram(_program);

		GLint link_status = GL_FALSE;
		_gl.GetProgramiv(_program, GL_LINK_STATUS, &link_status);
		if (!link_status)
			return false; // TODO: _gl.GetProgramInfoLog.

		_linked = true;
		return true;
	}

	bool GlGpuProgram::set_fragment_shader(Language language, const StaticString& source)
	{
		return set_shader(_fragment_shader, GL_FRAGMENT_SHADER, language, source);
	}

	bool GlGpuProgram::set_vertex_shader(Language language, const StaticString& source)
	{
		return set_shader(_vertex_shader, GL_VERTEX_SHADER, language, source);
	}

	bool GlGpuProgram::set_shader(GLuint& result, GLenum type, Language language, const StaticString& source) const
	{
		if (result)
			return false; // TODO: Throw?

		if (language != Language::Glsl)
			return false;

		const auto shader = _gl.CreateShader(type);
		if (!shader)
			return false; // TODO: Throw.

		const GLchar* source_text = source.text();
		const GLint source_size = source.size();
		_gl.ShaderSource(shader, 1, &source_text, &source_size);
		_gl.CompileShader(shader);

		GLint compile_status = GL_FALSE;
		_gl.GetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
		if (!compile_status)
		{
#if Y_IS_DEBUG
			char buffer[1024] = {};
			GLsizei length = 0;
			_gl.GetShaderInfoLog(shader, sizeof buffer, &length, buffer);
			Log() << buffer;
#endif
			_gl.DeleteShader(shader);
			return false; // TODO: _gl.GetShaderInfoLog.
		}

		_gl.AttachShader(_program, shader);

		result = shader;
		return true;
	}
}
