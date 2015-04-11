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
		void clear() override;
		void take_screenshot(Image&) override;
		void flush_2d_impl() override;
		bool initialize() override;
		void set_debug_texture_impl() override;
		void set_projection(const Matrix4f&) override;
		void update_window_size() override;

	private:

		bool check_min_version(int major, int minor);

	private:

		GlApi  _gl;
		GLuint _debug_texture = 0;
	};
}

#endif // __RENDERER_GL_RENDERER_H
