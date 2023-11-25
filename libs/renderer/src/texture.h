// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/renderer/texture.h>

#include <yttrium/geometry/size.h>

#include <seir_image/image.hpp>

namespace Yt
{
	class RectF;
	class RenderBackend;

	class BackendTexture2D : public Texture2D
	{
	public:
		BackendTexture2D(RenderBackend&, const seir::ImageInfo&, bool has_mipmaps);

		Size size() const noexcept override { return _size; }

		RectF full_rectangle() const;
		seir::ImageAxes orientation() const noexcept { return _orientation; }

	protected:
		RenderBackend& _backend;
		const Size _size;
		const seir::ImageAxes _orientation;
		const bool _has_mipmaps;
	};
}
