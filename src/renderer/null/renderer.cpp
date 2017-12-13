#include "renderer.h"

#include <yttrium/renderer/program.h>
#include "../texture.h"

namespace Yttrium
{
	std::unique_ptr<RenderProgram> NullRenderer::create_program(RendererImpl&, const std::string&, const std::string&)
	{
		struct NullProgram : RenderProgram
		{
			void set_uniform(const std::string&, const Matrix4&) override {}
		};
		return std::make_unique<NullProgram>();
	}

	std::unique_ptr<Texture2D> NullRenderer::create_texture_2d(RendererImpl& renderer, Image&& image, Flags<RenderManager::TextureFlag> flags)
	{
		const auto has_mipmaps = !(flags & RenderManager::TextureFlag::NoMipmaps);
		return std::make_unique<BackendTexture2D>(renderer, image.format(), has_mipmaps);
	}

	Image NullRenderer::take_screenshot(const Size& window_size) const
	{
		return Image{{window_size, PixelFormat::Rgb24, 4, ImageOrientation::XRightYDown}};
	}
}
