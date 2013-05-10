#include <Yttrium/renderer/texture_cache.h>

#include <Yttrium/buffer.h>
#include <Yttrium/image.h>
#include <Yttrium/string.h>

namespace Yttrium
{

TextureCache::TextureCache(const Renderer &renderer)
	: Object(renderer.allocator())
	, _renderer(renderer)
{
}

TextureCache::~TextureCache()
{
}

Texture2D TextureCache::cache_texture_2d(const StaticString &name, bool intensity)
{
	Allocator *allocator = _renderer.allocator();

	Image image(allocator);

	if (!image.load(name))
		return Texture2D();

	if (intensity && image.format().pixel_format() == PixelFormat::Gray)
		image.intensity_to_bgra();

	Texture2D::Private *texture_private = cache_texture_2d(image.format(), image.const_data());
	if (!texture_private)
		return Texture2D();

	return _cache_2d.insert(Cache2D::value_type(String(name, allocator), Texture2D(texture_private))).first->second;
}

void TextureCache::clear()
{
	_cache_2d.clear();
}

Texture2D TextureCache::load_texture_2d(const StaticString &name, bool intensity)
{
	Cache2D::iterator i = _cache_2d.find(String(name, ByReference()));
	return i != _cache_2d.end() ? i->second : cache_texture_2d(name, intensity);
}

} // namespace Yttrium
