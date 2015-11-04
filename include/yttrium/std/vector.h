/// \file
/// \brief <tt>&lt;vector&gt;</tt> containers with %Yttrium allocators.

#ifndef _include_yttrium_std_vector_h_
#define _include_yttrium_std_vector_h_

#include <yttrium/std/allocator.h>

#include <vector>

namespace Yttrium
{
	template <typename T>
	using StdVector = std::vector<T, StdAllocator<T>>;
}

#endif
