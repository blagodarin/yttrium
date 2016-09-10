#ifndef _src_renderer_texture_h_
#define _src_renderer_texture_h_

#include <yttrium/renderer/texture.h>

#include <yttrium/image.h>

namespace Yttrium
{
	class RendererImpl;

	class BackendTexture2D : public Texture2D
	{
	public:
		BackendTexture2D(RendererImpl& renderer, const ImageFormat& format, bool has_mipmaps);
		~BackendTexture2D() override;

		RectF full_rectangle() const;
		ImageOrientation orientation() const { return _orientation; }

	protected:
		RendererImpl& _renderer;
		const ImageOrientation _orientation;
		const bool _has_mipmaps;
	};
}

#endif
