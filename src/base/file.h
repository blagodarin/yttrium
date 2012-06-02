#ifndef __BASE_FILE_H
#define __BASE_FILE_H

#include <Yttrium/global.h>

#if Y_IS_POSIX
	#include "posix/file.h"
#elif Y_IS_WINDOWS
	#include "windows/file.h"
#endif

#endif // __BASE_FILE_H
