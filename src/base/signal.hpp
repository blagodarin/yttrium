#ifndef __BASE_SIGNAL_HPP
#define __BASE_SIGNAL_HPP

#include <Yttrium/global.hpp>

#if Y_IS_POSIX
	#include "posix/signal.hpp"
#elif Y_IS_WINDOWS
	#include "windows/signal.hpp"
#endif

#endif // __BASE_SIGNAL_HPP
