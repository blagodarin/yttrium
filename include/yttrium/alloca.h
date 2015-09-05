/// \file
/// \brief Stack memory allocation.

#ifndef __Y_ALLOCA_H
#define __Y_ALLOCA_H

#include <yttrium/global.h>

/// \def Y_ALLOCA
/// \brief Allocates memory on the stack.
/// \return Pointer to the allocated memory.
#if Y_COMPILER_GCC
	#define Y_ALLOCA(size) __builtin_alloca(size)
#elif Y_COMPILER_MSVC
	#include <malloc.h>
	#define Y_ALLOCA(size) _alloca(size)
#else
	#error Y_ALLOCA is unavailable for your compiler.
#endif

#endif
