#ifndef __BASE_FILE_HPP
#define __BASE_FILE_HPP

#include "../config.hpp"

#if __Y_POSIX
	#include "posix/file.hpp"
#elif __Y_WIN32
	#include "win32/file.hpp"
#endif

#endif // __BASE_FILE_HPP
