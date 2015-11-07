#ifndef _src_renderer_gl_gpu_program_h_
#define _src_renderer_gl_gpu_program_h_

#include <yttrium/gpu_program.h>

#include "gl.h"

namespace Yttrium
{
	class Allocator;
	class RendererImpl;
	class StaticString;
	class String;

	class GlShaderHandle
	{
	public:

		GlShaderHandle(const GlApi& gl, GLenum type);
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

	class GlProgramHandle
	{
	public:

		GlProgramHandle(const GlApi& gl);
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

	class GlGpuProgram : public GpuProgram
	{
	public:

		GlGpuProgram(RendererImpl& renderer, GlShaderHandle&& vertex_shader, GlShaderHandle&& fragment_shader, const GlApi& gl);
		~GlGpuProgram() override;

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
