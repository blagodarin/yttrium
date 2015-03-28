#ifndef __RENDERER_GL_TEXTURE_H
#define __RENDERER_GL_TEXTURE_H

#include "../texture.h"
#include "gl.h"

namespace Yttrium
{

class GlTexture2D: public BackendTexture2D
{
public:

	GlTexture2D(const ImageFormat& format, Allocator* allocator, const GlApi &gl, GLenum target, GLuint texture);

	~GlTexture2D() override;

private:

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
