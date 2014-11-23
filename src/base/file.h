#ifndef __BASE_FILE_H
#define __BASE_FILE_H

#include <yttrium/global.h>

#if Y_PLATFORM_POSIX
	#include "posix/file.h"
#elif Y_PLATFORM_WINDOWS
	#include "windows/file.h"
#endif

#endif // __BASE_FILE_H
