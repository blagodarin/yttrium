// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../../../../application/src/opengl.h"

#include <string_view>
#include <vector>

#ifdef _WIN32
#	include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glext.h>

namespace Yt
{
	class GlApi
	{
	public:
#include "api.h"

		GlApi();

		GlApi(const GlApi&) = delete;
		GlApi& operator=(const GlApi&) = delete;

	private:
		bool has_extension(std::string_view) const;
	};
}
