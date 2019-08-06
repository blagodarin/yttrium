//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "renderer.h"

#include <yttrium/renderer/program.h>
#include "../texture.h"

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

	Image NullRenderer::take_screenshot(const Size& window_size) const
	{
		return Image{ { static_cast<std::size_t>(window_size._width), static_cast<std::size_t>(window_size._height), PixelFormat::Rgb24, 4, ImageOrientation::XRightYDown } };
	}
}
