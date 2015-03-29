#ifndef __WINDOW_BACKEND_H
#define __WINDOW_BACKEND_H

#include <yttrium/global.h>

#if Y_PLATFORM_POSIX
	#include "x11/screen.h"
	#include "x11/window.h"
#endif

#endif // __WINDOW_BACKEND_H
