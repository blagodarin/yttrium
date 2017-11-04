#ifndef _src_renderer_vulkan_renderer_h_
#define _src_renderer_vulkan_renderer_h_

#include "../backend.h"
#include "context.h"

namespace Yttrium
{
	enum class VA;
	class VulkanVertexFormat;

	class VulkanRenderer final : public RendererBackend
	{
	public:
		explicit VulkanRenderer(const WindowBackend&);
		~VulkanRenderer() noexcept override;

		VulkanContext& context() noexcept { return _context; }

		void clear() override;
		std::unique_ptr<GpuProgram> create_builtin_program_2d(RendererImpl&) override;
		std::unique_ptr<GpuProgram> create_gpu_program(RendererImpl&, const std::string& vertex_shader, const std::string& fragment_shader) override;
		std::unique_ptr<Mesh> create_mesh(const MeshData&) override;
		std::unique_ptr<Texture2D> create_texture_2d(RendererImpl&, Image&&, Flags<Renderer::TextureFlag>) override;
		size_t draw_mesh(const Mesh&) override;
		void flush_2d(const Buffer&, const Buffer&) override;
		RectF map_rect(const RectF&, ImageOrientation) const override;
		void set_program(const GpuProgram*) override;
		void set_texture(const Texture2D&, Flags<Texture2D::Filter>) override;
		void set_window_size(const Size&) override;
		Image take_screenshot(const Size&) const override;

	private:
		const VulkanVertexFormat& vertex_format(const std::vector<VA>&);

	private:
		VulkanContext _context;
		std::vector<std::pair<std::vector<VA>, std::unique_ptr<const VulkanVertexFormat>>> _vertex_format_cache;
	};
}

#endif
