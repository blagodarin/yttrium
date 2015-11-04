/// \file
/// \brief <tt>&lt;map&gt;</tt> containers with %Yttrium allocators.

#ifndef _include_yttrium_std_map_h_
#define _include_yttrium_std_map_h_

#include <yttrium/std/allocator.h>

#include <map>

namespace Yttrium
{
	template <typename T1, typename T2, typename Less = std::less<T1>>
	using StdMap = std::map<T1, T2, Less, StdAllocator<std::pair<const T1, T2>>>;

	template <typename T1, typename T2, typename Less = std::less<T1>>
	using StdMultimap = std::multimap<T1, T2, Less, StdAllocator<std::pair<const T1, T2>>>;
}

#endif
