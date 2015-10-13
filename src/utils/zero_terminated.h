#ifndef _src_utils_zero_terminated_h_
#define _src_utils_zero_terminated_h_

#include <yttrium/global.h>

#include <cstring>

#if Y_COMPILER_GCC
	#define Y_ALLOCA(size) __builtin_alloca(size)
#elif Y_COMPILER_MSVC
	#include <malloc.h>
	#define Y_ALLOCA(size) _alloca(size)
#else
	#error Y_ALLOCA is unavailable for your compiler.
#endif

#define Y_ZERO_TERMINATED(result, string) \
	const char* const result = string.text(); \
	if (result[string.size()] != '\0') \
	{ \
		const_cast<const char*&>(result) = static_cast<char*>(Y_ALLOCA(string.size() + 1));\
		::memcpy(const_cast<char*>(result), string.text(), string.size()); \
		const_cast<char*>(result)[string.size()] = '\0'; \
	} \

#endif
