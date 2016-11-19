#ifndef _src_system_stderr_h_
#define _src_system_stderr_h_

#include <yttrium/global.h>

#if Y_PLATFORM_POSIX
	#include "posix/stderr.h"
#else
	#error Not implemented.
#endif

#endif
