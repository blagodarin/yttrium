#ifndef _src_platform_window_h_
#define _src_platform_window_h_

#if defined(_WIN32)
	#include "gdi/window.h"
#elif Y_RENDERER_OPENGL
	#include "x11/window.h"
#else
	#include "xcb/window.h"
#endif

#endif
