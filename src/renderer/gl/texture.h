#ifndef _src_renderer_gl_texture_h_
#define _src_renderer_gl_texture_h_

#include "../texture.h"
#include "wrappers.h"

namespace Yttrium
{
	class GlTexture2D : public BackendTexture2D
	{
	public:
		GlTexture2D(RendererImpl&, const ImageFormat&, bool has_mipmaps, GlTextureHandle&&);

		void bind() const;

	private:
		const GlTextureHandle _texture;
	};
}

#endif
