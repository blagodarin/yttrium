#include "texture.h"

#include "renderer.h"

namespace Yttrium
{
	BackendTexture2D::BackendTexture2D(RendererImpl& renderer, const ImageFormat& format, bool has_mipmaps)
		: Texture2D(format.size(), &renderer.allocator())
		, _renderer(renderer)
		, _orientation(format.orientation())
		, _has_mipmaps(has_mipmaps)
	{
	}

	BackendTexture2D::~BackendTexture2D()
	{
		_renderer.forget_texture(this);
	}
}
