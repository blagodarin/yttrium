// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "builtin.h"

#include <yttrium/exceptions.h>
#include <yttrium/image/image.h>
#include <yttrium/renderer/program.h>
#include "backend.h"

namespace
{
	const int32_t _white_texture_data = -1;
}

namespace Yt
{
	RenderBuiltin::RenderBuiltin(RenderBackend& backend)
		: _white_texture{ backend.create_texture_2d({ { 1, 1, PixelFormat::Bgra32 }, &_white_texture_data }, RenderManager::TextureFlag::NoMipmaps) }
		, _program_2d{ backend.create_builtin_program_2d() }
	{
		if (!_white_texture)
			throw InitializationError("Failed to initialize an internal texture");
		if (!_program_2d)
			throw InitializationError("Failed to initialize an internal rendering program");
	}

	RenderBuiltin::~RenderBuiltin() noexcept = default;
}
