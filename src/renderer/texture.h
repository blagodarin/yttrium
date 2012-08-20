#ifndef __RENDERER_TEXTURE_H
#define __RENDERER_TEXTURE_H

#include <Yttrium/image.h>
#include <Yttrium/renderer/texture_cache.h>

#include "../base/private_base.h"

namespace Yttrium
{

class Y_PRIVATE Texture2D::Private: public PrivateBase<Texture2D::Private>
{
	friend class Renderer;

public:

	Private(const TextureCache &cache, const ImageFormat &format, Allocator *allocator)
		: PrivateBase(allocator)
		, _cache(cache)
		, _size(format.width, format.height)
		, _filter(Texture2D::NearestFilter)
		, _orientation(format.orientation)
		, _has_mipmaps(true)
	{
	}

	virtual ~Private()
	{
	}

public:

	virtual void set_filter(Texture2D::Filter filter);

public:

	TextureCache      _cache;
	Dim2              _size;
	Texture2D::Filter _filter;
	ImageOrientation  _orientation;
	bool              _has_mipmaps;
};

} // namespace Yttrium

#endif // __RENDERER_TEXTURE_H
