#include "texture_cache.h"

#include "renderer.h"

namespace Yttrium
{

TextureCache::TextureCache(const TextureCache &cache)
	: _private(cache._private)
{
	_private->_renderer = Renderer::Private::copy(_private->_renderer);
}

TextureCache::~TextureCache()
{
	Renderer::Private::release(&_private->_renderer);
}

void TextureCache::clear()
{
	_private->_cache_2d.clear();
}

Texture2D TextureCache::load_texture_2d(const StaticString &name)
{
	Private::Cache2D::iterator i = _private->_cache_2d.find(String(name, String::Ref));

	return i != _private->_cache_2d.end()
		? i->second
		: cache_texture_2d(name);
}

Texture2D TextureCache::cache_texture_2d(const StaticString &name)
{
	Allocator *allocator = _private->_renderer->_allocator;

	Texture2D::Private *texture_private = nullptr;

	ImageReader image(name, ImageType::Auto, allocator);

	if (image.is_opened())
	{
		const ImageFormat &format = image.format();

		Buffer buffer(format.frame_size(), allocator);

		if (image.read(buffer.data()))
		{
			texture_private = _private->cache_texture_2d(*this, format, buffer.const_data(), allocator);

			if (texture_private)
			{
				Texture2D result(texture_private);

				_private->_cache_2d[name] = result;
				return result;
			}
		}
	}

	return Texture2D();
}

TextureCache &TextureCache::operator =(const TextureCache &cache)
{
	if (Y_LIKELY(_private != cache._private))
	{
		Renderer::Private::release(&_private->_renderer);
		_private = cache._private;
		_private->_renderer = Renderer::Private::copy(_private->_renderer);
	}

	return *this;
}

TextureCache::TextureCache(Private *private_)
	: _private(private_)
{
	_private->_renderer = Renderer::Private::copy(_private->_renderer);
}

} // namespace Yttrium
