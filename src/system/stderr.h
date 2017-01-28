#ifndef _src_system_stderr_h_
#define _src_system_stderr_h_

#include "../config.h"
#ifdef Y_IS_POSIX
	#include "posix/stderr.h"
#elif defined(Y_IS_WINDOWS)
	#include "windows/stderr.h"
#else
	#error Not implemented
#endif

#endif
