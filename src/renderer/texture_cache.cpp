#include "texture_cache.h"

#include <yttrium/image.h>
#include <yttrium/string.h>

#include "texture.h"

namespace Yttrium
{

BackendTextureCache::BackendTextureCache(const Renderer &renderer)
	: TextureCache(renderer.allocator())
	, _renderer(renderer)
{
}

BackendTextureCache::~BackendTextureCache()
{
}

Texture2DPtr BackendTextureCache::cache_texture_2d(const StaticString &name, bool intensity)
{
	Allocator *allocator = _renderer.allocator();

	Image image(allocator);

	if (!image.load(name))
		return Texture2DPtr();

	if (intensity && image.format().pixel_format() == PixelFormat::Gray)
		image.intensity_to_bgra();

	BackendTexture2D *backend_texture = cache_texture_2d(image.format(), image.const_data());
	if (!backend_texture)
		return Texture2DPtr();

	return _cache_2d.emplace(String(name, allocator), Texture2DPtr(backend_texture)).first->second;
}

void BackendTextureCache::clear()
{
	_cache_2d.clear();
}

Texture2DPtr BackendTextureCache::load_texture_2d(const StaticString &name, bool intensity)
{
	Cache2D::iterator i = _cache_2d.find(String(name, ByReference()));
	return i != _cache_2d.end() ? i->second : cache_texture_2d(name, intensity);
}

} // namespace Yttrium
