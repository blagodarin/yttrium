#ifndef __RENDERER_GL_TEXTURE_CACHE_H
#define __RENDERER_GL_TEXTURE_CACHE_H

#include "../texture_cache.h"

#include "gl.h"

namespace Yttrium
{

class OpenGlTextureCache: public TextureCache::Private
{
public:

	OpenGlTextureCache(const GlApi &gl)
		: _gl(gl)
	{
	}

public:

	virtual Texture2D::Private *cache_texture_2d(const TextureCache &cache, const ImageFormat &format, const void *data, Allocator *allocator);

public:

	const GlApi &_gl;
};

} // namespace Yttrium

#endif // __RENDERER_GL_TEXTURE_CACHE_H
