#ifndef __BASE_SYSTEM_ALLOCATOR_HPP
#define __BASE_SYSTEM_ALLOCATOR_HPP

#include "../config.hpp"

#if __Y_POSIX
	#include "posix/system_allocator.hpp"
#elif __Y_WIN32
	#include "win32/system_allocator.hpp"
#endif

#endif // __BASE_SYSTEM_ALLOCATOR_HPP
