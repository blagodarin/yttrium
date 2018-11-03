#pragma once

#if defined(_WIN32)
	#include "windows/window.h"
#elif Y_RENDERER_OPENGL
	#include "x11/window.h"
#else
	#include "xcb/window.h"
#endif
