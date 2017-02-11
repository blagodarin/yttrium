#include "gpu_program.h"

#include "renderer.h"

#ifndef NDEBUG
	#include <iostream>
#endif

namespace Yttrium
{
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

	void GlGpuProgram::set_uniform(const std::string& name, const Matrix4& value)
	{
		_program.set_uniform(name.c_str(), value);
	}

	bool GlGpuProgram::link()
	{
		if (_program.link())
			return true;
#ifndef NDEBUG
		std::cerr << _program.info_log() << "\n";
#endif
		return false;
	}
}
