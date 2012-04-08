#ifndef __BASE_THREAD_HPP
#define __BASE_THREAD_HPP

#include <Yttrium/global.hpp>

#if Y_IS_POSIX
	#include "posix/thread.hpp"
#elif Y_IS_WINDOWS
	#include "windows/thread.hpp"
#endif

#endif // __BASE_THREAD_HPP
