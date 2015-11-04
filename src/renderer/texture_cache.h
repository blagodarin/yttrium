#ifndef _src_renderer_texture_cache_h_
#define _src_renderer_texture_cache_h_

#include <yttrium/texture_cache.h>

#include <yttrium/std/map.h>

namespace Yttrium
{
	class RendererImpl;
	class String;

	class TextureCacheImpl : public TextureCache
	{
	public:

		TextureCacheImpl(RendererImpl& renderer);

		void clear() override;
		SharedPtr<Texture2D> load_texture_2d(const StaticString& name, bool intensity) override;

	private:

		RendererImpl& _renderer;
		StdMap<String, SharedPtr<Texture2D>> _cache_2d;
	};
}

#endif
