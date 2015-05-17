#ifndef __RENDERER_GL_TEXTURE_H
#define __RENDERER_GL_TEXTURE_H

#include "../texture.h"
#include "gl.h"

namespace Yttrium
{
	class GlTexture2D : public BackendTexture2D
	{
	public:

		GlTexture2D(RendererImpl& renderer, const ImageFormat& format, bool has_mipmaps, const GlApi& gl, GLuint texture);
		~GlTexture2D() override;

		void bind() const;

	private:

		Vector2 fix_coords(const Vector2& coords) const override;

	private:

		const GlApi& _gl;
		GLuint       _texture;
	};
}

#endif // __RENDERER_GL_TEXTURE_H
