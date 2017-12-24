#ifndef _src_renderer_renderer_h_
#define _src_renderer_renderer_h_

#include <yttrium/renderer/manager.h>

#include <memory>

namespace Yttrium
{
	enum class ImageOrientation;
	class RectF;
	class RenderBackend;
	class Size;
	class WindowBackend;

	class RendererImpl final : public RenderManager
	{
	public:
		explicit RendererImpl(WindowBackend&);
		~RendererImpl() override;

		std::unique_ptr<RenderProgram> create_program(const std::string& vertex_shader, const std::string& fragment_shader) override;
		std::unique_ptr<Texture2D> create_texture_2d(Image&&, Flags<TextureFlag>) override;
		std::unique_ptr<Mesh> load_mesh(const Source&) override;

	public:
		RectF map_rect(const RectF&, ImageOrientation) const;
		void set_window_size(const Size&);
		Image take_screenshot(const Size&) const;

	public:
		const std::unique_ptr<RenderBackend> _backend;
	};
}

#endif
