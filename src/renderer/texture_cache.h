#ifndef __RENDERER_TEXTURE_CACHE_H
#define __RENDERER_TEXTURE_CACHE_H

#include <yttrium/image.h>
#include <yttrium/renderer.h>
#include <yttrium/renderer/texture_cache.h>

#include <map>

namespace Yttrium
{

class BackendTexture2D;

class BackendTextureCache: public TextureCache
{
protected:

	BackendTextureCache(Renderer& renderer);
	~BackendTextureCache() override;

protected:

	virtual BackendTexture2D *cache_texture_2d(const ImageFormat &format, const void *data) = 0;

protected:

	Renderer& _renderer;

private:

	Texture2DPtr cache_texture_2d(const StaticString &name, bool intensity) override;

	void clear() override;

	Texture2DPtr load_texture_2d(const StaticString &name, bool intensity) override;

private:

	std::map<String, Texture2DPtr> _cache_2d;
};

} // namespace Yttrium

#endif // __RENDERER_TEXTURE_CACHE_H
