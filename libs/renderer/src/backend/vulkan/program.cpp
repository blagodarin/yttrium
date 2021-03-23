// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "program.h"

#include <yttrium/math/matrix.h>
#include "renderer.h"

namespace Yt
{
	void VulkanProgram::set_uniform(const std::string&, const Matrix4& matrix)
	{
		_renderer.update_uniforms(&matrix, sizeof matrix); // TODO: Fix this hack.
	}
}
