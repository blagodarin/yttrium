// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "texture.h"

#include <yttrium/math/rect.h>
#include "backend.h"

namespace Yt
{
	BackendTexture2D::BackendTexture2D(RenderBackend& backend, const ImageInfo& info, bool has_mipmaps)
		: _backend{ backend }
		, _size{ info.width(), info.height() }
		, _orientation{ info.orientation() }
		, _has_mipmaps{ has_mipmaps }
	{
	}

	RectF BackendTexture2D::full_rectangle() const
	{
		return _backend.map_rect({ {}, SizeF(_size) }, _orientation);
	}
}
