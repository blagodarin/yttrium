#include "texture_cache.h"

#include <yttrium/image.h>
#include <yttrium/string.h>
#include "renderer.h"

namespace Yttrium
{
	std::unique_ptr<TextureCache> TextureCache::create(Renderer& renderer)
	{
		return std::make_unique<TextureCacheImpl>(static_cast<RendererImpl&>(renderer));
	}

	TextureCacheImpl::TextureCacheImpl(RendererImpl& renderer)
		: _renderer(renderer)
	{
	}

	void TextureCacheImpl::clear()
	{
		_cache_2d.clear();
	}

	Pointer<Texture2D> TextureCacheImpl::load_texture_2d(const StaticString& name, bool intensity)
	{
		const auto i = _cache_2d.find(String(name, ByReference()));
		if (i != _cache_2d.end())
			return i->second;

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
}
