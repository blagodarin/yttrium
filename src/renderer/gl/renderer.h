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
		UniquePtr<GpuProgram> create_gpu_program(const StaticString& vertex_shader, const StaticString& fragment_shader) override;
		UniquePtr<IndexBuffer> create_index_buffer(IndexFormat, size_t, const void*) override;
		ResourcePtr<Texture2D> create_texture_2d(const Image&, bool no_mipmaps) override;
		UniquePtr<VertexBuffer> create_vertex_buffer(std::initializer_list<VA>, size_t, const void*) override;
		void draw_mesh(const Mesh&) override;
		void draw_triangles(const VertexBuffer&, const IndexBuffer&) override;

		// RendererImpl
		void clear() override;
		ResourcePtr<Mesh> create_mesh(const MeshData&) override;
		RectF map_rect(const RectF&, ImageOrientation) const override;
		Image take_screenshot() const override;
		void flush_2d_impl(const Buffer&, const Buffer&) override;
		void set_program(const GpuProgram*) override;
		void set_texture(const Texture2D&, Texture2D::Filter) override;
		void set_window_size_impl(const Size&) override;

	private:
#ifndef NDEBUG
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
