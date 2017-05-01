// TODO-17: Remove this header.

#ifndef _include_yttrium_std_optional_h_
#define _include_yttrium_std_optional_h_

#ifdef _MSC_VER
	#include <optional>
#elif __has_include(<optional>)
	#include <optional>
#elif __has_include(<experimental/optional>)
	#include <experimental/optional>
	namespace std
	{
		using std::experimental::optional;
	}
#else
	#error Missing required header: <optional> or <experimental/optional>.
#endif

#endif
