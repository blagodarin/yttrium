#ifndef __RENDERER_TEXTURE_CACHE_H
#define __RENDERER_TEXTURE_CACHE_H

#include <Yttrium/image.h>
#include <Yttrium/renderer.h>
#include <Yttrium/string.h>

#include <map>

namespace Yttrium
{

class TextureCache::Private
{
public:

	typedef std::map<String, Texture2D> Cache2D;

public:

	Renderer::Private *_renderer;
	Cache2D            _cache_2d;

public:

	Private()
		: _renderer(nullptr)
	{
	}

	virtual ~Private()
	{
	}

public:

	virtual Texture2D::Private *cache_texture_2d(const TextureCache &cache, const ImageFormat &format, const void *data, Allocator *allocator) = 0;
};

} // namespace Yttrium

#endif // __RENDERER_TEXTURE_CACHE_H
