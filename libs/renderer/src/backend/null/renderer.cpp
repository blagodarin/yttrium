// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "renderer.h"

#include <yttrium/renderer/program.h>
#include "../../texture.h"

#include <seir_image/image.hpp>

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

	std::unique_ptr<Texture2D> NullRenderer::create_texture_2d(const seir::ImageInfo& info, const void*, Flags<RenderManager::TextureFlag> flags)
	{
		const auto has_mipmaps = !(flags & RenderManager::TextureFlag::NoMipmaps);
		return std::make_unique<BackendTexture2D>(*this, info, has_mipmaps);
	}

	seir::Image NullRenderer::take_screenshot(const Size& viewport_size) const
	{
		const seir::ImageInfo info{ width, height, seir::PixelFormat::Rgb24, seir::ImageAxes::XRightYDown };
		seir::Buffer buffer{ info.frameSize() };
		return seir::Image{ info, std::move(buffer) };
	}
}
