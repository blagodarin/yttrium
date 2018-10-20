#ifndef _include_yttrium_utils_algorithm_h_
#define _include_yttrium_utils_algorithm_h_

namespace Yttrium
{
	///
	template <typename I, typename P>
	constexpr I forward_find_if(I iterator, const P& predicate)
	{
		for (; !predicate(*iterator); ++iterator);
		return iterator;
	}
}

#endif
