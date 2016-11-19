#ifndef _src_system_window_h_
#define _src_system_window_h_

#include <yttrium/global.h>

#if Y_PLATFORM_POSIX
	#include "x11/window.h"
#else
	#error Not implemented.
#endif

#endif
