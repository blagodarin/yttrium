#include "renderer.h"

#include <yttrium/renderer/gpu_program.h>
#include <yttrium/renderer/mesh.h>
#include "../mesh_data.h"
#include "../texture.h"

#include <cassert>

namespace Yttrium
{
	class NullGpuProgram final : public GpuProgram
	{
	public:
		void set_uniform(const std::string&, const Matrix4&) override {}
	};

	std::unique_ptr<GpuProgram> NullRenderer::create_gpu_program(const std::string&, const std::string&)
	{
		return std::make_unique<NullGpuProgram>();
	}

	std::unique_ptr<Texture2D> NullRenderer::create_texture_2d(Image&& image, Flags<TextureFlag> flags)
	{
		const auto has_mipmaps = !(flags & TextureFlag::NoMipmaps);
		return std::make_unique<BackendTexture2D>(*this, image.format(), has_mipmaps);
	}

	std::unique_ptr<Mesh> NullRenderer::create_mesh(const MeshData& data)
	{
		assert(!data._vertex_format.empty());
		assert(data._vertex_data.size() > 0);
		assert(!data._indices.empty());

		return std::make_unique<Mesh>();
	}

	Image NullRenderer::take_screenshot() const
	{
		return Image{{window_size(), PixelFormat::Rgb, 24, 4, ImageOrientation::XRightYDown}};
	}
}
