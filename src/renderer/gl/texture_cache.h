#ifndef __RENDERER_GL_TEXTURE_CACHE_H
#define __RENDERER_GL_TEXTURE_CACHE_H

#include <Yttrium/renderer/texture_cache.h>

namespace Yttrium
{

struct GlApi;

class OpenGlTextureCache: public TextureCache
{
public:

	OpenGlTextureCache(const Renderer &renderer, const GlApi &gl);

private: // TextureCache.

	virtual Texture2D::Private *cache_texture_2d(const ImageFormat &format, const void *data) noexcept;

private:

	const GlApi &_gl;
};

} // namespace Yttrium

#endif // __RENDERER_GL_TEXTURE_CACHE_H
