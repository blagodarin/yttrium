#ifndef __BASE_CONDITION_H
#define __BASE_CONDITION_H

#include <yttrium/global.h>

#if Y_IS_POSIX
	#include "posix/condition.h"
#elif Y_IS_WINDOWS
	#include "windows/condition.h"
#endif

#endif // __BASE_CONDITION_H
