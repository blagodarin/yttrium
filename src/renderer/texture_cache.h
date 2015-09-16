#ifndef __RENDERER_TEXTURE_CACHE_H
#define __RENDERER_TEXTURE_CACHE_H

#include <yttrium/texture_cache.h>

#include <map>

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
		std::map<String, SharedPtr<Texture2D>> _cache_2d;
	};
}

#endif
