#ifndef __RENDERER_TEXTURE_CACHE_H
#define __RENDERER_TEXTURE_CACHE_H

#include <yttrium/texture_cache.h>

#include <yttrium/image.h>

#include <map>

namespace Yttrium
{
	class BackendTexture2D;
	class RendererImpl;

	class BackendTextureCache: public TextureCache
	{
	protected:

		BackendTextureCache(RendererImpl& renderer);
		~BackendTextureCache() override;

		virtual BackendTexture2D* cache_texture_2d(const ImageFormat& format, const void* data) = 0;

	protected:

		RendererImpl& _renderer;

	private:

		Pointer<Texture2D> cache_texture_2d(const StaticString& name, bool intensity) override;
		void clear() override;
		Pointer<Texture2D> load_texture_2d(const StaticString& name, bool intensity) override;

	private:

		std::map<String, Pointer<Texture2D>> _cache_2d;
	};
}

#endif // __RENDERER_TEXTURE_CACHE_H
