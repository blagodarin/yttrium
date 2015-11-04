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

	RectF BackendTexture2D::full_rectangle() const
	{
		const Vector2& top_left = fix_coords(Vector2(0, 0));
		const Vector2& bottom_right = fix_coords(Vector2(_size));
		return RectF::from_coords(top_left.x, top_left.y, bottom_right.x, bottom_right.y);
	}
}
