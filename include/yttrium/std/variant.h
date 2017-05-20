// TODO-17: Remove this header.

#ifndef _include_yttrium_std_variant_h_
#define _include_yttrium_std_variant_h_

#ifdef _MSC_VER
	#include <variant>
#elif __has_include(<variant>)
	#include <variant>
#elif __has_include(<experimental/variant>)
	#include <experimental/variant>
	namespace std
	{
		using std::experimental::variant;
	}
#elif __has_include(<boost/variant.hpp>)
	#include <boost/variant/apply_visitor.hpp>
	#include <boost/variant/variant.hpp>
	namespace std
	{
		using boost::variant;
		using monostate = boost::blank;
		template <class Visitor, class... Variants>
		decltype(auto) visit(Visitor&& visitor, Variants&&... variants)
		{
			return boost::apply_visitor(std::forward<Visitor>(visitor), std::forward<Variants>(variants)...);
		}
	}
#else
	#error Missing required header: <variant>, <experimental/variant> or <boost/variant.hpp>.
#endif

#endif
