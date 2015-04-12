#include "texture.h"

namespace Yttrium
{
	BackendTexture2D::BackendTexture2D(const ImageFormat& format, Allocator* allocator)
		: Texture2D(format.size(), allocator)
		, _orientation(format.orientation())
		, _has_mipmaps(true)
	{
	}

	RectF BackendTexture2D::full_rectangle() const
	{
		const Vector2& top_left = fix_coords(Vector2(0, 0));
		const Vector2& bottom_right = fix_coords(Vector2(_size));
		return RectF::from_coords(top_left.x, top_left.y, bottom_right.x, bottom_right.y);
	}
}
