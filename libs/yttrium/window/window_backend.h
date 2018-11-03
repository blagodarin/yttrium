#pragma once

#if defined(_WIN32)
	#include "../platform/windows/window.h"
#elif Y_RENDERER_OPENGL
	#include "../platform/x11/window.h"
#else
	#include "../platform/xcb/window.h"
#endif
