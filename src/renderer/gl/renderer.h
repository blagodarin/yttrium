#ifndef _src_renderer_gl_renderer_h_
#define _src_renderer_gl_renderer_h_

#include "../renderer.h"
#include "wrappers.h"

namespace Yttrium
{
	class GlRenderer final : public RendererImpl
	{
	public:
		GlRenderer();

		// Renderer
		std::unique_ptr<GpuProgram> create_gpu_program(const std::string& vertex_shader, const std::string& fragment_shader) override;
		std::unique_ptr<IndexBuffer> create_index_buffer(IndexFormat, size_t, const void*) override;
		std::unique_ptr<Texture2D> create_texture_2d(Image&&, Flags<TextureFlag>) override;
		std::unique_ptr<VertexBuffer> create_vertex_buffer(const std::vector<VA>&, size_t, const void*) override;
		void draw_mesh(const Mesh&) override;
		void draw_triangles(const VertexBuffer&, const IndexBuffer&) override;

		// RendererImpl
		void clear() override;
		std::unique_ptr<Mesh> create_mesh(const MeshData&) override;
		RectF map_rect(const RectF&, ImageOrientation) const override;
		Image take_screenshot() const override;
		void flush_2d_impl(const Buffer&, const Buffer&) override;
		void set_program(const GpuProgram*) override;
		void set_texture(const Texture2D&, Flags<Texture2D::Filter>) override;
		void set_window_size_impl(const Size&) override;

	private:
#ifndef NDEBUG
		void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) const;
#endif

	private:
		const GlApi _gl;
		GlBufferHandle _2d_ibo{ _gl, GL_ELEMENT_ARRAY_BUFFER };
		GlBufferHandle _2d_vbo{ _gl, GL_ARRAY_BUFFER };
		GlVertexArrayHandle _2d_vao{ _gl };
	};
}

#endif
