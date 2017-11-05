#ifndef _src_renderer_null_renderer_h_
#define _src_renderer_null_renderer_h_

#include <yttrium/math/rect.h>
#include <yttrium/renderer/mesh.h>
#include "../backend.h"

namespace Yttrium
{
	class WindowBackend;

	class NullRenderer final : public RendererBackend
	{
	public:
		explicit NullRenderer(const WindowBackend&) {}

		void clear() override {}
		std::unique_ptr<GpuProgram> create_builtin_program_2d(RendererImpl& renderer) override { return create_gpu_program(renderer, {}, {}); }
		std::unique_ptr<GpuProgram> create_gpu_program(RendererImpl&, const std::string&, const std::string&) override;
		std::unique_ptr<Mesh> create_mesh(const MeshData&) override { return std::make_unique<Mesh>(); }
		std::unique_ptr<Texture2D> create_texture_2d(RendererImpl&, Image&&, Flags<Renderer::TextureFlag>) override;
		size_t draw_mesh(const Mesh&) override { return 0; }
		void flush_2d(const Buffer&, const Buffer&) override {}
		RectF map_rect(const RectF& rect, ImageOrientation) const override { return rect; }
		void set_program(const GpuProgram*) override {}
		void set_texture(const Texture2D&, Flags<Texture2D::Filter>) override {}
		void set_window_size(const Size&) override {}
		Image take_screenshot(const Size&) const override;
	};
}

#endif
