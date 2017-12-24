#ifndef _src_renderer_gl_texture_h_
#define _src_renderer_gl_texture_h_

#include "../texture.h"
#include "wrappers.h"

namespace Yttrium
{
	class GlTexture2D final : public BackendTexture2D
	{
	public:
		GlTexture2D(RenderBackend&, const ImageFormat&, bool has_mipmaps, GlTextureHandle&&);

		void bind(Flags<Texture2D::Filter>) const;

	private:
		const GlTextureHandle _texture;
	};
}

#endif
