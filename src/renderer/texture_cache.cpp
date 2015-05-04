#include "texture_cache.h"

#include <yttrium/string.h>
#include "renderer.h"

namespace Yttrium
{
	TextureCacheImpl::TextureCacheImpl(RendererImpl& renderer)
		: _renderer(renderer)
	{
	}

	Pointer<Texture2D> TextureCacheImpl::cache_texture_2d(const StaticString &name, bool intensity)
	{
		Allocator* allocator = _renderer.allocator();

		Image image(allocator);

		if (!image.load(name))
			return {};

		if (intensity && image.format().pixel_format() == PixelFormat::Gray)
			image.intensity_to_bgra();

		const auto& backend_texture = _renderer.create_texture_2d(image.format(), image.const_data());
		if (!backend_texture)
			return {};

		return _cache_2d.emplace(String(name, allocator), backend_texture).first->second;
	}

	void TextureCacheImpl::clear()
	{
		_cache_2d.clear();
	}

	Pointer<Texture2D> TextureCacheImpl::load_texture_2d(const StaticString& name, bool intensity)
	{
		const auto i = _cache_2d.find(String(name, ByReference()));
		return i != _cache_2d.end() ? i->second : cache_texture_2d(name, intensity);
	}
}
