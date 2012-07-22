#ifndef __TERMINAL_WINDOW_H
#define __TERMINAL_WINDOW_H

#include <Yttrium/global.h>

#if Y_IS_POSIX
	#include "x11/window.h"
#elif Y_IS_WINDOWS
	#include "windows/window.h"
#endif

#endif // __TERMINAL_WINDOW_H
