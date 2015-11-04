/// \file
/// \brief <tt>&lt;unordered_map&gt;</tt> containers with %Yttrium allocators.

#ifndef _include_yttrium_std_unordered_map_h_
#define _include_yttrium_std_unordered_map_h_

#include <yttrium/std/allocator.h>

#include <unordered_map>

namespace Yttrium
{
	template <typename T1, typename T2, typename Hash = std::hash<T1>, typename EqualTo = std::equal_to<T1>>
	using StdUnorderedMap = std::unordered_map<T1, T2, Hash, EqualTo, StdAllocator<std::pair<const T1, T2>>>;

	template <typename T1, typename T2, typename Hash = std::hash<T1>, typename EqualTo = std::equal_to<T1>>
	using StdUnorderedMultimap = std::unordered_multimap<T1, T2, Hash, EqualTo, StdAllocator<std::pair<const T1, T2>>>;
}

#endif
