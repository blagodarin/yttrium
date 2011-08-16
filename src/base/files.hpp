#ifndef __BASE_FILES_HPP
#define __BASE_FILES_HPP

#include "../config.hpp"

#if __Y_POSIX
	#include "posix/files.hpp"
#elif __Y_WIN32
	#include "win32/files.hpp"
#endif

#endif // __BASE_FILES_HPP
