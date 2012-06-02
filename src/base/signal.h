#ifndef __BASE_SIGNAL_H
#define __BASE_SIGNAL_H

#include <Yttrium/global.h>

#if Y_IS_POSIX
	#include "posix/signal.h"
#elif Y_IS_WINDOWS
	#include "windows/signal.h"
#endif

#endif // __BASE_SIGNAL_H
