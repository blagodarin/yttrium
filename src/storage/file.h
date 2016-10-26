#ifndef _src_storage_file_h_
#define _src_storage_file_h_

#include <yttrium/global.h>

#if Y_PLATFORM_POSIX
	#include "posix/file.h"
#else
	#error Not implemented.
#endif

#endif
