#ifndef __RENDERER_TEXTURE_H
#define __RENDERER_TEXTURE_H

#include <yttrium/texture.h>

#include <yttrium/image.h>

namespace Yttrium
{
	class BackendTexture2D: public Texture2D
	{
	public:

		BackendTexture2D(Allocator* allocator, const ImageFormat& format, bool has_mipmaps);

		virtual Vector2 fix_coords(const Vector2& coords) const = 0; // TODO: fix_rectangle().

		RectF full_rectangle() const;

	protected:

		const ImageOrientation _orientation;
		bool                   _has_mipmaps;
	};
}

#endif // __RENDERER_TEXTURE_H
