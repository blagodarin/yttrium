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

Texture2D TextureCache::cache_texture_2d(const StaticString &name)
{
	Allocator *allocator = _renderer.allocator();

	ImageReader image(name, ImageType::Auto, allocator);
	if (!image.is_opened())
		return Texture2D();

	const ImageFormat &format = image.format();

	Buffer buffer(format.frame_size(), allocator);
	if (!image.read(buffer.data()))
		return Texture2D();

	Texture2D::Private *texture_private = cache_texture_2d(format, buffer.const_data());
	if (!texture_private)
		return Texture2D();

	return _cache_2d.insert(Cache2D::value_type(String(name, allocator), Texture2D(texture_private))).first->second;
}

void TextureCache::clear()
{
	_cache_2d.clear();
}

Texture2D TextureCache::load_texture_2d(const StaticString &name)
{
	Cache2D::iterator i = _cache_2d.find(String(name, ByReference()));
	return i != _cache_2d.end() ? i->second : cache_texture_2d(name);
}

} // namespace Yttrium
