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

		// Renderer
		std::unique_ptr<IndexBuffer> create_index_buffer(IndexBuffer::Format, size_t, const void*) override;
		Pointer<Texture2D> create_texture_2d(const ImageFormat&, const void*) override;
		std::unique_ptr<VertexBuffer> create_vertex_buffer(unsigned format, size_t, const void*) override;
		void draw_triangles(const VertexBuffer&, const IndexBuffer&) override;

		// RendererImpl
		void clear() override;
		void take_screenshot(Image&) override;
		void flush_2d_impl() override;
		bool initialize() override;
		void set_projection(const Matrix4&) override;
		void set_texture(const BackendTexture2D*) override;
		void set_transformation(const Matrix4&) override;
		void update_window_size() override;

	private:

		bool check_min_version(int major, int minor);
#if Y_IS_DEBUG
		void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) const;
#endif

	private:

		GlApi _gl;
	};
}

#endif // __RENDERER_GL_RENDERER_H
