/// \file
/// \brief

#ifndef __Y_ALLOCA_H
#define __Y_ALLOCA_H

#include <yttrium/global.h>

///
#if Y_COMPILER_GCC
	#define Y_ALLOCA(size) __builtin_alloca(size)
#elif Y_COMPILER_MSVC
	#include <malloc.h>
	#define Y_ALLOCA(size) _alloca(size)
#else
	#error
#endif

#endif // __Y_ALLOCA_H
