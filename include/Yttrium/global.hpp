/// \file
/// \brief Global definitions.
/// \note This header is included (directly or indirectly) in all other %Yttrium headers.

// All the macros that start with __Y except __Y_DEBUG are internal and should
// only be used in this header. As such, they are pseudo-documented by the ordinary
// comments to provide some information about them, but hide them from the doxygen.

#ifndef __Y_GLOBAL_HPP
#define __Y_GLOBAL_HPP

// \def __YTTRIUM
// \brief %Yttrium compilation "flag".
// \note This should be defined by the build system when compiling the %Yttrium itself.

/******************************************************************************\
* 1) Detect the compiler version and filter the unsupported compilers.
\******************************************************************************/

// \def __Y_GCC
// \brief GCC version number (e.g. 43 for GCC 4.3).
// \note The minimal supported GCC version is 4.3.

#if defined(__GNUC__)
	#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)
		#define __Y_GCC (__GNUC__ * 10 + __GNUC_MINOR__)
	#else
		#error GCC compilers older than 4.3 are not supported.
	#endif
#else
	#define __Y_GCC 0
#endif

// \def __Y_MSVC
// \brief MSVC version number (e.g. 71 for MSVC 7.1, 100 for MSVC 10.0).
// \note The minimal supported MSVC version is 10.0.

#if defined(_MSC_VER)
	#if _MSC_VER >= 1700
		#define __Y_MSVC 110
	#elif _MSC_VER >= 1600
		#define __Y_MSVC 100
	#else
		#error MSVC compilers older than 10.0 are not supported.
	#endif
#else
	#define __Y_MSVC 0
#endif

// The above compilers should suffice.

#if !(__Y_GCC || __Y_MSVC)
	#error Only the GCC and MSVC compilers are supported.
#endif

/******************************************************************************\
* 2) Neutralize the supported compilers' differences.
\******************************************************************************/

// __func__ for MSVC (in MSVC 10.0, __func__ is still not supported).

#if __Y_MSVC && !defined(__func__)
	#define __func__ __FUNCTION__
#endif

// nullptr for older versions of GCC.

#if __Y_GCC && __Y_GCC < 46 && !defined(nullptr)
	#define nullptr __null
#endif

/// \def Y_API
/// \brief Public API specifier.

/// \def Y_PRIVATE
/// \brief Private definition specifier.

// GCC (thus MinGW) supports __declspec(dllexport) and __declspec(dllimport).

#if defined(_WIN32) || defined(__CYGWIN__)
	#ifdef __YTTRIUM
		#define Y_API __declspec(dllexport)
	#else
		#define Y_API __declspec(dllimport)
	#endif
	#define Y_PRIVATE
#elif __Y_GCC
	#define Y_API     __attribute__((visibility("default")))
	#define Y_PRIVATE __attribute__((visibility("hidden")))
#else
	#error Unsupported compiler and target platform combination.
#endif

/******************************************************************************\
* 3) Provide some globally useful compiler-independent definitions.
\******************************************************************************/

/// Calculate the static \a array length.

#define Y_LENGTH_OF(array) (sizeof(array) / sizeof (array)[0])

/// %Yttrium namespace.

namespace Yttrium
{
} // namespace Yttrium

#endif // __Y_GLOBAL_HPP
