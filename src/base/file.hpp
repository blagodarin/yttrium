#ifndef __BASE_FILE_HPP
#define __BASE_FILE_HPP

#include <Yttrium/global.hpp>

#if Y_POSIX
	#include "posix/file.hpp"
#elif Y_WINDOWS
	#include "windows/file.hpp"
#endif

#endif // __BASE_FILE_HPP
