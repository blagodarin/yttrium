#ifndef _src_renderer_texture_h_
#define _src_renderer_texture_h_

#include <yttrium/texture.h>

#include <yttrium/image.h>

namespace Yttrium
{
	class RendererImpl;

	class BackendTexture2D: public Texture2D
	{
	public:

		BackendTexture2D(RendererImpl& renderer, const ImageFormat& format, bool has_mipmaps);
		~BackendTexture2D() override;

		virtual Vector2 fix_coords(const Vector2& coords) const = 0; // TODO: fix_rectangle().

		RectF full_rectangle() const;

	protected:

		RendererImpl&          _renderer;
		const ImageOrientation _orientation;
		bool                   _has_mipmaps;
	};
}

#endif
