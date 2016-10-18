#include <yttrium/renderer/texture_cache.h>

#include <yttrium/image.h>
#include <yttrium/io/reader.h>
#include <yttrium/io/storage.h>
#include <yttrium/memory/shared_ptr.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/resource/loader.h>
#include <yttrium/std/map.h>
#include <yttrium/string.h>

namespace Yttrium
{
	class TextureCacheImpl : public TextureCache
	{
	public:
		TextureCacheImpl(ResourceLoader&, Allocator&);

		void clear() override;
		SharedPtr<Texture2D> load_texture_2d(const StaticString& name, bool intensity) override;

	private:
		ResourceLoader& _resource_loader;
		Allocator& _allocator;
		StdMap<String, SharedPtr<Texture2D>> _cache_2d{ _allocator }; // TODO: Use weak pointers.
	};

	TextureCacheImpl::TextureCacheImpl(ResourceLoader& resource_loader, Allocator& allocator)
		: _resource_loader(resource_loader)
		, _allocator(allocator)
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
		auto texture = _resource_loader.load_texture_2d(name, intensity);
		if (!texture)
			return {};
		return _cache_2d.emplace(String(name, &_allocator), std::move(texture)).first->second;
	}

	UniquePtr<TextureCache> TextureCache::create(ResourceLoader& resource_loader, Allocator& allocator)
	{
		return make_unique<TextureCacheImpl>(allocator, resource_loader, allocator);
	}
}
