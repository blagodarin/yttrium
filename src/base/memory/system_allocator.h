#ifndef __BASE_MEMORY_SYSTEM_ALLOCATOR_H
#define __BASE_MEMORY_SYSTEM_ALLOCATOR_H

#include <Yttrium/global.h>

#if Y_IS_POSIX
	#include "../posix/system_allocator.h"
#elif Y_IS_WINDOWS
	#include "../windows/system_allocator.h"
#endif

#endif // __BASE_MEMORY_SYSTEM_ALLOCATOR_H
