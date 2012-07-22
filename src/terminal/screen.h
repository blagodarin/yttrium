#ifndef __TERMINAL_SCREEN_H
#define __TERMINAL_SCREEN_H

#include <Yttrium/global.h>

#if Y_IS_POSIX
	#include "x11/screen.h"
#elif Y_IS_WINDOWS
	#include "windows/screen.h"
#endif

#endif // __TERMINAL_SCREEN_H
