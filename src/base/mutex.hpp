#ifndef __BASE_MUTEX_HPP
#define __BASE_MUTEX_HPP

#include <Yttrium/global.hpp>

#if Y_IS_POSIX
	#include "posix/mutex.hpp"
#elif Y_IS_WINDOWS
	#include "windows/mutex.hpp"
#endif

#endif // __BASE_MUTEX_HPP
