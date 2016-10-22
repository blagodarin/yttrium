#ifndef _src_renderer_texture_h_
#define _src_renderer_texture_h_

#include <yttrium/renderer/texture.h>

#include <yttrium/image.h>

namespace Yttrium
{
	class RectF;
	class RendererImpl;

	class BackendTexture2D : public Texture2D
	{
	public:
		BackendTexture2D(RendererImpl& renderer, const ImageFormat& format, bool has_mipmaps);
		~BackendTexture2D() override;

		Size size() const noexcept override { return _size; }

		RectF full_rectangle() const;
		ImageOrientation orientation() const { return _orientation; }

	protected:
		RendererImpl& _renderer;
		const Size _size;
		const ImageOrientation _orientation;
		const bool _has_mipmaps;
	};
}

#endif
