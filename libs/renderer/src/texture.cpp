// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "texture.h"

#include "backend/backend.h"

#include <seir_graphics/rectf.hpp>

namespace Yt
{
	BackendTexture2D::BackendTexture2D(RenderBackend& backend, const seir::ImageInfo& info, bool has_mipmaps)
		: _backend{ backend }
		, _size{ size_t{ info.width() }, size_t{ info.height() } }
		, _orientation{ info.axes() }
		, _has_mipmaps{ has_mipmaps }
	{
	}

	seir::RectF BackendTexture2D::full_rectangle() const
	{
		return _backend.map_rect({ {}, seir::SizeF{ _size } }, _orientation);
	}
}
