/// \file
/// \brief Stack memory allocation.

#ifndef _include_yttrium_alloca_h_
#define _include_yttrium_alloca_h_

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
