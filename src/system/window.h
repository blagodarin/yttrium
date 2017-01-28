#ifndef _src_system_window_h_
#define _src_system_window_h_

#include "../config.h"
#ifdef Y_IS_POSIX
	#include "x11/window.h"
#elif defined(Y_IS_WINDOWS)
	#include "gdi/window.h"
#else
	#error Not implemented
#endif

#endif
