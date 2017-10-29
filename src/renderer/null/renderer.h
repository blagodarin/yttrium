#ifndef _src_renderer_null_renderer_h_
#define _src_renderer_null_renderer_h_

#include "../renderer.h"

namespace Yttrium
{
	class NullRenderer final : public RendererImpl
	{
	public:
		explicit NullRenderer(const WindowBackend&) {}

		// Renderer
		std::unique_ptr<GpuProgram> create_gpu_program(const std::string&, const std::string&) override;
		std::unique_ptr<Texture2D> create_texture_2d(Image&&, Flags<TextureFlag>) override;
		void draw_mesh(const Mesh&) override {}

		// RendererImpl
		void clear() override {}
		std::unique_ptr<GpuProgram> create_builtin_program_2d() override;
		std::unique_ptr<Mesh> create_mesh(const MeshData&) override;
		RectF map_rect(const RectF& rect, ImageOrientation) const override { return rect; }
		Image take_screenshot() const override;
		void flush_2d_impl(const Buffer&, const Buffer&) override {}
		void set_program(const GpuProgram*) override {}
		void set_texture(const Texture2D&, Flags<Texture2D::Filter>) override {}
		void set_window_size_impl(const Size&) override {}
	};
}

#endif
