// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef _WIN32
#	define GLAPI_CALL __stdcall
#else
#	define GLAPI_CALL
#endif

namespace Yt
{
	using GlAddress = void(GLAPI_CALL*)();
	GlAddress gl_address(const char* name);
}
