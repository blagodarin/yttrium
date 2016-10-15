#ifndef _src_renderer_texture_cache_h_
#define _src_renderer_texture_cache_h_

#include <yttrium/renderer/texture_cache.h>

#include <yttrium/std/map.h>

namespace Yttrium
{
	class RendererImpl;
	class String;

	class TextureCacheImpl : public TextureCache
	{
	public:
		TextureCacheImpl(const Storage&, RendererImpl&);

		void clear() override;
		SharedPtr<Texture2D> load_texture_2d(const StaticString& name, bool intensity) override;

	private:
		const Storage& _storage;
		RendererImpl& _renderer;
		StdMap<String, SharedPtr<Texture2D>> _cache_2d;
	};
}

#endif
