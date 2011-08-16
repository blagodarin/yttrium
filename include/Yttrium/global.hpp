/// \file
/// \brief Global definitions.
/// \note This header is included (directly or indirectly) by all other %Yttrium headers.

#ifndef __Y_GLOBAL_HPP
#define __Y_GLOBAL_HPP

#include <cstddef> // NULL

// I don't believe one will use GCC 4.2 or older, but it is always better to check.
// In MSVC 2010 (_MSC_VER == 1600), __func__ is still not supported.

#if (defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 3))) || defined(_MSC_VER)
	#define __func__ __FUNCTION__
#endif

/// \def Y_API
/// \brief %Yttrium API specifier.

#ifndef Y_API
	#ifdef _MSC_VER
		#define Y_API __declspec(dllimport)
	#else
		#define Y_API
	#endif
#endif

#define Y_LENGTH_OF(x) (sizeof(x) / sizeof (x)[0])

#define __Y_STR(x) #x

/// Stringification wrapper.

#define Y_STR(x) __Y_STR(x)

/// %Yttrium namespace.

namespace Yttrium
{
} // namespace Yttrium

#endif // __Y_GLOBAL_HPP
