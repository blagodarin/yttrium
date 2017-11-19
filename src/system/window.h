#ifndef _src_system_window_h_
#define _src_system_window_h_

#include "../config.h"
#if Y_IS_POSIX
	#ifdef Y_RENDERER_OPENGL
		#include "x11/window.h"
	#else
		#include "xcb/window.h"
	#endif
#elif Y_IS_WINDOWS
	#include "gdi/window.h"
#else
	#error Not implemented
#endif

#endif
