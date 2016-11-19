#ifndef _src_base_std_err_h_
#define _src_base_std_err_h_

#include <yttrium/global.h>

#if Y_PLATFORM_POSIX
	#include "posix/std_err.h"
#else
	#error Not implemented.
#endif

#endif
