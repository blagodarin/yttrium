#ifndef __BASE_MUTEX_H
#define __BASE_MUTEX_H

#include <Yttrium/global.h>

#if Y_IS_POSIX
	#include "posix/mutex.h"
#elif Y_IS_WINDOWS
	#include "windows/mutex.h"
#endif

#endif // __BASE_MUTEX_H
