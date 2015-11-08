#include "gpu_program.h"

#include <yttrium/log.h>
#include "renderer.h"

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
