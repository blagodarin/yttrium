#ifndef __BASE_MEMORY_SYSTEM_ALLOCATOR_HPP
#define __BASE_MEMORY_SYSTEM_ALLOCATOR_HPP

#include <Yttrium/global.hpp>

#if Y_IS_POSIX
	#include "../posix/system_allocator.hpp"
#elif Y_IS_WINDOWS
	#include "../windows/system_allocator.hpp"
#endif

#endif // __BASE_MEMORY_SYSTEM_ALLOCATOR_HPP
