#ifndef _src_renderer_vulkan_renderer_h_
#define _src_renderer_vulkan_renderer_h_

#include "../renderer.h"
#include "wrappers.h"

namespace Yttrium
{
	class VulkanRenderer final : public RendererImpl
	{
	public:
		VulkanRenderer(const WindowBackend& window) : _window{window} {}

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
		const WindowBackend& _window;
		VulkanInstance _instance;
		VulkanSurface _surface{_instance.get(), _window};
	};
}

#endif
