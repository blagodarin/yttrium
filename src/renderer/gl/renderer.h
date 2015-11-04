#ifndef _src_renderer_gl_renderer_h_
#define _src_renderer_gl_renderer_h_

#include "../renderer.h"
#include "gl.h"

namespace Yttrium
{
	class GLRenderer : public RendererImpl
	{
	public:

		GLRenderer(WindowBackend& window, Allocator& allocator);

		// Renderer
		Pointer<GpuProgram> create_gpu_program() override;
		Pointer<IndexBuffer> create_index_buffer(IndexBuffer::Format, size_t, const void*) override;
		SharedPtr<Texture2D> create_texture_2d(const ImageFormat&, const void*, bool no_mipmaps) override;
		Pointer<VertexBuffer> create_vertex_buffer(unsigned format, size_t, const void*) override;
		void draw_triangles(const VertexBuffer&, const IndexBuffer&) override;

		// RendererImpl
		void clear() override;
		void take_screenshot(Image&) override;
		void flush_2d_impl(const StdVector<Vertex2D>&, const StdVector<uint16_t>&) override;
		bool initialize() override;
		void set_program(const GpuProgram*) override;
		void set_projection(const Matrix4&) override;
		void set_texture(const Texture2D*) override;
		void set_transformation(const Matrix4&) override;
		void set_window_size_impl(const Size&) override;

	private:

		bool check_min_version(int major, int minor);
#if Y_IS_DEBUG
		void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) const;
#endif

	private:

		GlApi _gl;
	};
}

#endif
