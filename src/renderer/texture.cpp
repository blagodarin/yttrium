#include "texture.h"

namespace Yttrium
{

BackendTexture2D::BackendTexture2D(const Renderer &renderer, const ImageFormat &format, Allocator *allocator)
	: Texture2D(Dim2(format.width(), format.height()), allocator)
	, _orientation(format.orientation())
	, _has_mipmaps(true)
	, _renderer(renderer)
{
}

RectF BackendTexture2D::full_rectangle() const
{
	const Vector2f &top_left = fix_coords(Vector2f(0));
	const Vector2f &bottom_right = fix_coords(_size);

	return RectF::from_coords(top_left.x, top_left.y, bottom_right.x, bottom_right.y);
}

} // namespace Yttrium
