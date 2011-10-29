/// \file
/// \brief Global definitions.
/// \note This header is included (directly or indirectly) by all other %Yttrium headers.

#ifndef __Y_GLOBAL_HPP
#define __Y_GLOBAL_HPP

#include <cstddef> // NULL

// I don't believe that someone will use GCC 4.2 or older, but it is always better to check.
// In MSVC 2010 (_MSC_VER == 1600), __func__ is still not supported.

#if !defined(__func__) && ((defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 3))) || defined(_MSC_VER))
	#define __func__ __FUNCTION__
#endif

/// \def Y_API
/// \brief %Yttrium API specifier.

// GCC (thus MinGW) supports __declspec(dllexport) and __declspec(dllimport).

#if defined(_WIN32) || defined(__CYGWIN__)
	#ifdef __Y_API_EXPORT
		#define Y_API __declspec(dllexport)
	#else
		#define Y_API __declspec(dllimport)
	#endif
	#define Y_PRIVATE
#else
	#if defined(__GNUC__) && __GNUC__ >= 4
		#define Y_API     __attribute__((visibility("default")))
		#define Y_PRIVATE __attribute__((visibility("hidden")))
	#else
		#define Y_API
		#define Y_PRIVATE
	#endif
#endif

///

#define Y_LENGTH_OF(x) (sizeof(x) / sizeof (x)[0])

/// %Yttrium namespace.

namespace Yttrium
{
} // namespace Yttrium

#endif // __Y_GLOBAL_HPP
