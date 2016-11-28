#ifndef _src_system_stderr_h_
#define _src_system_stderr_h_

#include "../config.h"
#ifdef Y_IS_POSIX
	#include "posix/stderr.h"
#else
	#error Not implemented.
#endif

#endif