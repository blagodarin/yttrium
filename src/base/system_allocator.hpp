#ifndef __BASE_SYSTEM_ALLOCATOR_HPP
#define __BASE_SYSTEM_ALLOCATOR_HPP

#include <Yttrium/global.hpp>

#if Y_POSIX
	#include "posix/system_allocator.hpp"
#elif Y_WINDOWS
	#include "windows/system_allocator.hpp"
#endif

#endif // __BASE_SYSTEM_ALLOCATOR_HPP
