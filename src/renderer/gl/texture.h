#ifndef _src_renderer_gl_texture_h_
#define _src_renderer_gl_texture_h_

#include "../texture.h"
#include "gl.h"

namespace Yttrium
{
	class GlTextureHandle
	{
	public:

		GlTextureHandle(const GlApi& gl, GLenum target);
		GlTextureHandle(GlTextureHandle&&);
		~GlTextureHandle();

		void bind() const;
		void set_anisotropy_enabled(bool) const;
		void set_parameter(GLenum, GLint) const;
		void unbind() const;

		GlTextureHandle(const GlTextureHandle&) = delete;
		GlTextureHandle& operator=(const GlTextureHandle&) = delete;
		GlTextureHandle& operator=(GlTextureHandle&&) = delete;

	private:

		const GlApi& _gl;
		const GLenum _target;
		GLuint _handle = 0;
	};

	class GlTexture2D : public BackendTexture2D
	{
	public:

		GlTexture2D(RendererImpl& renderer, const ImageFormat& format, bool has_mipmaps, GlTextureHandle&& texture);

		void bind() const;

	private:

		Vector2 map(const Vector2&) const override;

	private:

		const GlTextureHandle _texture;
	};
}

#endif
