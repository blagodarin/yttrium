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

#include "builtin.h"

#include <yttrium/exceptions.h>
#include <yttrium/image.h>
#include <yttrium/renderer/program.h>
#include "../backend.h"
#include "debug_texture.h"

namespace
{
	const int32_t _white_texture_data = -1;
}

namespace Yttrium
{
	RenderBuiltin::RenderBuiltin(RenderBackend& backend)
		: _white_texture{ backend.create_texture_2d({ { 1, 1, PixelFormat::Bgra32 }, &_white_texture_data }, RenderManager::TextureFlag::NoMipmaps) }
		, _debug_texture{ backend.create_texture_2d({ { DebugTexture::width, DebugTexture::height, PixelFormat::Bgra32 }, DebugTexture::data }, RenderManager::TextureFlag::NoMipmaps) }
		, _program_2d{ backend.create_builtin_program_2d() }
	{
		if (!_white_texture || !_debug_texture)
			throw InitializationError("Failed to initialize an internal texture");
		if (!_program_2d)
			throw InitializationError("Failed to initialize an internal rendering program");
	}

	RenderBuiltin::~RenderBuiltin() noexcept = default;
}
