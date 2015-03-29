#ifndef __RENDERER_GL_RENDERER_H
#define __RENDERER_GL_RENDERER_H

#include "../renderer.h"
#include "gl.h"

namespace Yttrium
{
	class OpenGlRenderer: public RendererImpl
	{
	public:

		OpenGlRenderer(WindowBackend& window, Allocator* allocator);
		~OpenGlRenderer() override;

		// Renderer
		std::unique_ptr<TextureCache> create_texture_cache() override;

		// RendererImpl
		bool initialize() override;
		void clear() override;
		void do_flush_2d() override;
		void do_set_matrix_2d(double width, double height) override;
		void set_viewport(const Dim2 &size) override;
		void take_screenshot() override;
		void bind_debug_texture() override;

	private:

		bool check_min_version(int major, int minor);

	public:

		GlApi  _gl;
		GLuint _debug_texture = 0;
	};
}

#endif // __RENDERER_GL_RENDERER_H
