#ifndef __RENDERER_BACKEND_GL_TEXTURE_CACHE_H
#define __RENDERER_BACKEND_GL_TEXTURE_CACHE_H

#include "../../texture_cache.h"

namespace Yttrium
{

struct GlApi;

class GlTextureCache: public BackendTextureCache
{
public:

	GlTextureCache(const Renderer &renderer, const GlApi &gl);

private:

	BackendTexture2D *cache_texture_2d(const ImageFormat &format, const void *data) noexcept override;

private:

	const GlApi &_gl;
};

} // namespace Yttrium

#endif // __RENDERER_BACKEND_GL_TEXTURE_CACHE_H
