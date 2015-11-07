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

		RectF full_rectangle() const
		{
			return map(RectF(0, 0, _size.width, _size.height));
		}

		RectF map(const RectF& rect) const
		{
			const auto& top_left = map(rect.top_left());
			const auto& bottom_right = map(rect.bottom_right());
			return RectF::from_coords(top_left.x, top_left.y, bottom_right.x, bottom_right.y);
		}

	protected:

		RendererImpl& _renderer;
		const ImageOrientation _orientation;
		const bool _has_mipmaps;

		virtual Vector2 map(const Vector2&) const = 0;
	};
}

#endif
