#ifndef _src_renderer_gl_renderer_h_
#define _src_renderer_gl_renderer_h_

#include "../renderer.h"
#include "wrappers.h"

namespace Yttrium
{
	class GlRenderer : public RendererImpl
	{
	public:
		GlRenderer(Allocator&);

		// Renderer
		Pointer<GpuProgram> create_gpu_program(const StaticString& vertex_shader, const StaticString& fragment_shader) override;
		Pointer<IndexBuffer> create_index_buffer(IndexFormat, size_t, const void*) override;
		SharedPtr<Texture2D> create_texture_2d(const ImageFormat&, const void*, bool no_mipmaps) override;
		Pointer<VertexBuffer> create_vertex_buffer(std::initializer_list<VA>, size_t, const void*) override;
		void draw_triangles(const VertexBuffer&, const IndexBuffer&) override;

		// RendererImpl
		void clear() override;
		RectF map_rect(const RectF&, ImageOrientation) const override;
		void take_screenshot(Image&) override;
		void flush_2d_impl(const Buffer&, const Buffer&) override;
		void set_program(const GpuProgram*) override;
		void set_texture(const Texture2D&) override;
		void set_window_size_impl(const Size&) override;

	private:
#if Y_IS_DEBUG
		void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) const;
#endif

	private:
		const GlApi _gl;
		GlBufferHandle _2d_ibo;
		GlBufferHandle _2d_vbo;
		GlVertexArrayHandle _2d_vao;
	};
}

#endif
