/// \file
/// \brief

#ifndef __RENDERER_GL_GPU_PROGRAM_H
#define __RENDERER_GL_GPU_PROGRAM_H

#include <yttrium/gpu_program.h>
#include "gl.h"

namespace Yttrium
{
	class GlGpuProgram : public GpuProgram
	{
	public:

		GlGpuProgram(const GlApi& gl);
		~GlGpuProgram() override;

		bool is_linked() const override { return _linked; }
		bool link() override;
		bool set_vertex_shader(Language, const StaticString&) override;
		bool set_fragment_shader(Language, const StaticString&) override;

		GLuint handle() const { return _program; }

	private:

		bool set_shader(GLuint& result, GLenum type, Language, const StaticString&) const;

	private:

		const GlApi& _gl;
		GLuint _program = 0;
		GLuint _vertex_shader = 0;
		GLuint _fragment_shader = 0;
		bool _linked = false;
	};
}

#endif
