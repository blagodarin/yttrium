/// \file
/// \brief

#ifndef _include_yttrium_memory_global_h_
#define _include_yttrium_memory_global_h_

#include <yttrium/api.h>

namespace Yttrium
{
	class Allocator;

	/// The default allocator.
	extern Y_API Allocator* const DefaultAllocator;

	/// An allocator that aborts the program when accessed,
	/// useful for preventing unwanted allocations.
	extern Y_API Allocator& NoAllocator;
}

#endif
