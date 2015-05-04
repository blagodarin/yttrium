#ifndef __RENDERER_TEXTURE_CACHE_H
#define __RENDERER_TEXTURE_CACHE_H

#include <yttrium/texture_cache.h>

#include <yttrium/image.h>

#include <map>

namespace Yttrium
{
	class RendererImpl;

	class TextureCacheImpl: public TextureCache
	{
	public:

		TextureCacheImpl(RendererImpl& renderer);

	private:

		Pointer<Texture2D> cache_texture_2d(const StaticString& name, bool intensity) override;
		void clear() override;
		Pointer<Texture2D> load_texture_2d(const StaticString& name, bool intensity) override;

	private:

		RendererImpl& _renderer;
		std::map<String, Pointer<Texture2D>> _cache_2d;
	};
}

#endif // __RENDERER_TEXTURE_CACHE_H
