#include "texture.h"

#include <yttrium/math/rect.h>
#include "backend.h"

namespace Yttrium
{
	BackendTexture2D::BackendTexture2D(RenderBackend& backend, const ImageFormat& format, bool has_mipmaps)
		: _backend{ backend }
		, _size{ format.size() }
		, _orientation{ format.orientation() }
		, _has_mipmaps{ has_mipmaps }
	{
	}

	RectF BackendTexture2D::full_rectangle() const
	{
		return _backend.map_rect({ {}, SizeF(_size) }, _orientation);
	}
}
