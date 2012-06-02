#ifndef __BASE_THREAD_H
#define __BASE_THREAD_H

#include <Yttrium/global.h>

#if Y_IS_POSIX
	#include "posix/thread.h"
#elif Y_IS_WINDOWS
	#include "windows/thread.h"
#endif

#endif // __BASE_THREAD_H
