// TODO-17: Remove this header.

#ifndef _include_yttrium_std_string_view_h_
#define _include_yttrium_std_string_view_h_

#ifdef _MSC_VER
	#include <string_view>
#elif __has_include(<string_view>)
	#include <string_view>
#elif __has_include(<experimental/string_view>)
	#include <experimental/string_view>
	namespace std
	{
		using std::experimental::string_view;
	}
#else
	#error Missing required header: <string_view> or <experimental/string_view>.
#endif

#endif
