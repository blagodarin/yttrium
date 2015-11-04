#include "texture_cache.h"

#include <yttrium/image.h>
#include <yttrium/string.h>
#include "renderer.h"

namespace Yttrium
{
	Pointer<TextureCache> TextureCache::create(Renderer& renderer)
	{
		RendererImpl& renderer_impl = static_cast<RendererImpl&>(renderer);
		return make_pointer<TextureCacheImpl>(renderer_impl.allocator(), static_cast<RendererImpl&>(renderer));
	}

	TextureCacheImpl::TextureCacheImpl(RendererImpl& renderer)
		: _renderer(renderer)
		, _cache_2d(_renderer.allocator())
	{
	}

	void TextureCacheImpl::clear()
	{
		_cache_2d.clear();
	}

	SharedPtr<Texture2D> TextureCacheImpl::load_texture_2d(const StaticString& name, bool intensity)
	{
		const auto i = _cache_2d.find(String(name, ByReference()));
		if (i != _cache_2d.end())
			return i->second;

		Allocator& allocator = _renderer.allocator();

		Image image(&allocator);
		if (!image.load(name))
			return {};

		if (intensity && image.format().pixel_format() == PixelFormat::Gray)
			image.intensity_to_bgra();

		const auto& backend_texture = _renderer.create_texture_2d(image.format(), image.const_data());
		if (!backend_texture)
			return {};

		return _cache_2d.emplace(String(name, &allocator), backend_texture).first->second;
	}
}
