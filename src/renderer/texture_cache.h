#ifndef __RENDERER_TEXTURE_CACHE_H
#define __RENDERER_TEXTURE_CACHE_H

#include <Yttrium/image.h>
#include <Yttrium/renderer.h>
#include <Yttrium/renderer/texture_cache.h>

#include <map>

namespace Yttrium
{

class BackendTexture2D;

class BackendTextureCache: public TextureCache
{
protected:

	BackendTextureCache(const Renderer &renderer);

	~BackendTextureCache() noexcept override;

protected:

	virtual BackendTexture2D *cache_texture_2d(const ImageFormat &format, const void *data) noexcept = 0;

protected:

	Renderer _renderer;

private:

	Texture2DPtr cache_texture_2d(const StaticString &name, bool intensity) noexcept override;

	void clear() noexcept override;

	Texture2DPtr load_texture_2d(const StaticString &name, bool intensity) noexcept override;

private:

	typedef std::map<String, Texture2DPtr> Cache2D;

	Cache2D _cache_2d;
};

} // namespace Yttrium

#endif // __RENDERER_TEXTURE_CACHE_H
