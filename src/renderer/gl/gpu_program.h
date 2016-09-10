#ifndef _src_renderer_gl_gpu_program_h_
#define _src_renderer_gl_gpu_program_h_

#include <yttrium/renderer/gpu_program.h>

#include "wrappers.h"

namespace Yttrium
{
	class Allocator;
	class RendererImpl;
	class StaticString;
	class String;

	class GlGpuProgram : public GpuProgram
	{
	public:
		GlGpuProgram(RendererImpl&, GlShaderHandle&& vertex_shader, GlShaderHandle&& fragment_shader, const GlApi&);
		~GlGpuProgram() override;

		void set_uniform(const StaticString&, const Matrix4&) override;

		GLuint handle() const { return _program.get(); }
		bool link();

	private:
		RendererImpl& _renderer;
		const GlShaderHandle _vertex_shader;
		const GlShaderHandle _fragment_shader;
		const GlProgramHandle _program;
	};
}

#endif
