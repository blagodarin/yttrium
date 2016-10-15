#include "texture_cache.h"

#include <yttrium/image.h>
#include <yttrium/io/reader.h>
#include <yttrium/io/storage.h>
#include <yttrium/memory/shared_ptr.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/string.h>
#include "renderer.h"

namespace Yttrium
{
	UniquePtr<TextureCache> TextureCache::create(const Storage& storage, Renderer& renderer)
	{
		RendererImpl& renderer_impl = static_cast<RendererImpl&>(renderer);
		return make_unique<TextureCacheImpl>(renderer_impl.allocator(), storage, renderer_impl);
	}

	TextureCacheImpl::TextureCacheImpl(const Storage& storage, RendererImpl& renderer)
		: _storage(storage)
		, _renderer(renderer)
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

		Image image;
		if (!image.load(_storage.open(name)))
			return {};

		if (intensity && image.format().pixel_format() == PixelFormat::Gray)
			image.intensity_to_bgra();

		auto&& backend_texture = _renderer.create_texture_2d(image.format(), image.data());
		if (!backend_texture)
			return {};

		return _cache_2d.emplace(String(name, &allocator), std::move(backend_texture)).first->second;
	}
}
