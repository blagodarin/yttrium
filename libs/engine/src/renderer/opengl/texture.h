// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/utils/flags.h>

#include "../texture.h"
#include "wrappers.h"

namespace Yt
{
	class GlTexture2D final : public BackendTexture2D
	{
	public:
		GlTexture2D(RenderBackend&, const ImageInfo&, bool has_mipmaps, GlTextureHandle&&);

		void bind(Flags<Texture2D::Filter>) const;

	private:
		const GlTextureHandle _texture;
	};
}
