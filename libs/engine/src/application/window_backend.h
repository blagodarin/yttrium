// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#if defined(_WIN32)
#	include "windows/window.h"
#elif Y_RENDERER_OPENGL
#	include "x11/window.h"
#else
#	include "xcb/window.h"
#endif
