#ifndef _src_base_file_h_
#define _src_base_file_h_

#include <yttrium/global.h>

#if Y_PLATFORM_POSIX
	#include "posix/file.h"
#elif Y_PLATFORM_WINDOWS
	#include "windows/file.h"
#endif

#endif
