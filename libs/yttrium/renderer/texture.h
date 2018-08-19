#ifndef _src_renderer_texture_h_
#define _src_renderer_texture_h_

#include <yttrium/renderer/texture.h>

#include <yttrium/image.h>

namespace Yttrium
{
	class RectF;
	class RenderBackend;

	class BackendTexture2D : public Texture2D
	{
	public:
		BackendTexture2D(RenderBackend&, const ImageFormat&, bool has_mipmaps);

		Size size() const noexcept override { return _size; }

		RectF full_rectangle() const;
		ImageOrientation orientation() const { return _orientation; }

	protected:
		RenderBackend& _backend;
		const Size _size;
		const ImageOrientation _orientation;
		const bool _has_mipmaps;
	};
}

#endif
