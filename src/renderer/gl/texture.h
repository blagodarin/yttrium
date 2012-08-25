#ifndef __RENDERER_GL_TEXTURE_H
#define __RENDERER_GL_TEXTURE_H

#include "../texture.h"

#include "gl.h"

namespace Yttrium
{

class OpenGlTexture2D: public Texture2D::Private
{
public:

	OpenGlTexture2D(const TextureCache &cache, const ImageFormat &format, Allocator *allocator,
		const GlApi &gl, GLenum target, GLuint texture)
		: Private(cache, format, allocator)
		, _gl(gl)
		, _target(target)
		, _texture(texture)
	{
	}

	virtual ~OpenGlTexture2D();

public:

	virtual void bind();

	virtual Vector2f fix_coords(const Vector2f &coords) const;

	virtual void unbind();

private:

	const GlApi &_gl;
	GLenum       _target;
	GLuint       _texture;
};

} // namespace Yttrium

#endif // __RENDERER_GL_TEXTURE_H
