#ifndef __RENDERER_GL_TEXTURE_CACHE_H
#define __RENDERER_GL_TEXTURE_CACHE_H

#include "../texture_cache.h"

namespace Yttrium
{
	struct GlApi;

	class GlTextureCache: public BackendTextureCache
	{
	public:

		GlTextureCache(Renderer& renderer, const GlApi& gl);

	private:

		BackendTexture2D* cache_texture_2d(const ImageFormat& format, const void* data) override;

	private:

		const GlApi &_gl;
	};
}

#endif // __RENDERER_GL_TEXTURE_CACHE_H
