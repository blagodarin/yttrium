#include "texture.h"

#include "renderer.h"

namespace Yttrium
{
	BackendTexture2D::BackendTexture2D(RendererImpl& renderer, const ImageFormat& format, bool has_mipmaps)
		: _renderer(renderer)
		, _size(format.size())
		, _orientation(format.orientation())
		, _has_mipmaps(has_mipmaps)
	{
	}

	BackendTexture2D::~BackendTexture2D()
	{
		_renderer.forget_texture(this);
	}

	RectF BackendTexture2D::full_rectangle() const
	{
		return _renderer.map_rect({{}, SizeF(_size)}, _orientation);
	}
}
