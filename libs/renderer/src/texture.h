// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/renderer/texture.h>

#include <seir_graphics/size.hpp>
#include <seir_image/image.hpp>

namespace seir
{
	class RectF;
}

namespace Yt
{
	class RenderBackend;

	class BackendTexture2D : public Texture2D
	{
	public:
		BackendTexture2D(RenderBackend&, const seir::ImageInfo&, bool has_mipmaps);

		seir::Size size() const noexcept override { return _size; }

		seir::RectF full_rectangle() const;
		seir::ImageAxes orientation() const noexcept { return _orientation; }

	protected:
		RenderBackend& _backend;
		const seir::Size _size;
		const seir::ImageAxes _orientation;
		const bool _has_mipmaps;
	};
}
