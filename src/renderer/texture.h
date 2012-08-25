#ifndef __RENDERER_TEXTURE_H
#define __RENDERER_TEXTURE_H

#include <Yttrium/image.h>
#include <Yttrium/rect.h>
#include <Yttrium/renderer/texture_cache.h>

#include "../base/private_base.h"

namespace Yttrium
{

class Y_PRIVATE Texture2D::Private: public PrivateBase<Texture2D::Private>
{
public:

	Private(const TextureCache &cache, const ImageFormat &format, Allocator *allocator);

	virtual ~Private()
	{
	}

public:

	virtual void bind() = 0;

	virtual Vector2f fix_coords(const Vector2f &coords) const = 0; // TODO: fix_rectangle().

	inline RectF full_rectangle() const;

	virtual void set_filter(Texture2D::Filter filter);

	virtual void unbind() = 0;

public:

	TextureCache      _cache;
	Dim2              _size;
	Texture2D::Filter _filter;
	ImageOrientation  _orientation;
	bool              _has_mipmaps;
};

////////////////////////////////////////////////////////////////////////////////

RectF Texture2D::Private::full_rectangle() const
{
	const Vector2f &top_left = fix_coords(Vector2f(0, 0));
	const Vector2f &outer_bottom_right = fix_coords(_size);

	return RectF::from_outer_coords(top_left.x, top_left.y,
		outer_bottom_right.x, outer_bottom_right.y);
}

} // namespace Yttrium

#endif // __RENDERER_TEXTURE_H
