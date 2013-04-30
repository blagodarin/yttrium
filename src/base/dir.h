#ifndef __BASE_DIR_H
#define __BASE_DIR_H

#include <Yttrium/global.h>

#if Y_IS_POSIX
	#include "posix/dir.h"
#elif Y_IS_WINDOWS
	#include "windows/dir.h"
#endif

#endif // __BASE_DIR_H
