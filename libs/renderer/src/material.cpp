// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "material.h"

#include <yttrium/renderer/program.h>

namespace Yt
{
	MaterialImpl::MaterialImpl(std::unique_ptr<RenderProgram>&& program, std::shared_ptr<const Texture2D>&& texture, Texture2D::Filter texture_filter)
		: _program{ std::move(program) }
		, _texture{ std::move(texture) }
		, _texture_filter{ texture_filter }
	{
	}
}
