#ifndef __RENDERER_GL_TEXTURE_H
#define __RENDERER_GL_TEXTURE_H

#include "../texture.h"

#include "gl.h"

namespace Yttrium
{

class OpenGlTexture2D
	: public Texture2D::Private
{
public:

	OpenGlTexture2D(const Renderer &renderer, const ImageFormat &format, Allocator *allocator,
		const GlApi &gl, GLenum target, GLuint texture)
		: Private(renderer, format, allocator)
		, _gl(gl)
		, _target(target)
		, _texture(texture)
	{
	}

	~OpenGlTexture2D() override;

public:

	void bind() override;

	Vector2f fix_coords(const Vector2f &coords) const override;

	void unbind() override;

private:

	const GlApi &_gl;
	GLenum       _target;
	GLuint       _texture;
};

} // namespace Yttrium

#endif // __RENDERER_GL_TEXTURE_H
