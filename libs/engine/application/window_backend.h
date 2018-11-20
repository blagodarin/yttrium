#pragma once

#if defined(_WIN32)
#	include "_windows/window.h"
#elif Y_RENDERER_OPENGL
#	include "_x11/window.h"
#else
#	include "_xcb/window.h"
#endif
