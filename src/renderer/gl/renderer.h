#ifndef __RENDERER_GL_RENDERER_H
#define __RENDERER_GL_RENDERER_H

#include "../renderer.h"
#include "gl.h"

namespace Yttrium
{
	class OpenGlRenderer: public Renderer::Private
	{
	public:

		OpenGlRenderer(WindowBackend& window, Allocator* allocator);
		~OpenGlRenderer() override;

		// Renderer::Private
		bool initialize() override;
		void bind_debug_texture() override;
		void clear() override;
		std::unique_ptr<TextureCache> create_texture_cache(Renderer&) override;
		void flush_2d() override;
		void set_matrix_2d(double width, double height) override;
		void set_viewport(const Dim2 &size) override;
		void take_screenshot() override;

	private:

		bool check_min_version(int major, int minor);

	public:

		GlApi  _gl;
		GLuint _debug_texture = 0;
	};
}

#endif // __RENDERER_GL_RENDERER_H
