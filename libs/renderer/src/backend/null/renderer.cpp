// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "renderer.h"

#include <yttrium/renderer/program.h>
#include "../../texture.h"

namespace Yt
{
	std::unique_ptr<RenderProgram> NullRenderer::create_program(const std::string&, const std::string&)
	{
		struct NullProgram : RenderProgram
		{
			void set_uniform(const std::string&, const Matrix4&) override {}
		};
		return std::make_unique<NullProgram>();
	}

	std::unique_ptr<Texture2D> NullRenderer::create_texture_2d(const Image& image, Flags<RenderManager::TextureFlag> flags)
	{
		const auto has_mipmaps = !(flags & RenderManager::TextureFlag::NoMipmaps);
		return std::make_unique<BackendTexture2D>(*this, image.info(), has_mipmaps);
	}

	Image NullRenderer::take_screenshot(const Size& viewport_size) const
	{
		return Image{ { static_cast<std::size_t>(viewport_size._width), static_cast<std::size_t>(viewport_size._height), PixelFormat::Rgb24, 4, ImageOrientation::XRightYDown } };
	}
}
