// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/renderer/texture.h>

#include <yttrium/image/image.h>
#include <yttrium/math/size.h>

namespace Yt
{
	class RectF;
	class RenderBackend;

	class BackendTexture2D : public Texture2D
	{
	public:
		BackendTexture2D(RenderBackend&, const ImageInfo&, bool has_mipmaps);

		Size size() const noexcept override { return _size; }

		RectF full_rectangle() const;
		ImageOrientation orientation() const noexcept { return _orientation; }

	protected:
		RenderBackend& _backend;
		const Size _size;
		const ImageOrientation _orientation;
		const bool _has_mipmaps;
	};
}
