/// \file
/// \brief Global definitions.
/// \note This header is included (directly or indirectly) in all other %Yttrium headers.

// All the macros that start with __Y are internal to a single header and should
// only be used in that header. As such, they are pseudo-documented by the ordinary
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
* 2) Detect the target platform.
\******************************************************************************/

// If it's not Windows, then it's definitely Linux. I do not need Macs,
// and I need FreeBSD etc. even less. =)

/// \def Y_IS_LINUX
/// \brief

#if defined(__linux__)
	#define Y_IS_LINUX 1
#else
	#define Y_IS_LINUX 0
#endif

/// \def Y_IS_POSIX
/// \brief Defined to 1 for a POSIX system, including Linux.

#if Y_IS_LINUX || defined(__unix__)
	#define Y_IS_POSIX 1
#else
	#define Y_IS_POSIX 0
#endif

/// \def Y_WINDOWS
/// \brief

#if defined(_WIN32) || defined(__CYGWIN__)
	#define Y_IS_WINDOWS 1
#else
	#define Y_IS_WINDOWS 0
#endif

// Check the correctness of the platform definitions.
// NOTE: Do we need this really?

#if Y_IS_POSIX && Y_IS_WINDOWS
	#error Invalid target platform.
#endif

#if !(Y_IS_POSIX || Y_IS_WINDOWS)
	#error Unknown target platform.
#endif

// Actually, Yttrium threats Windows as Windows and Linux as POSIX + X11 combo.
// NOTE: There is one exception, namely SystemAllocator, which is for Linux.

/******************************************************************************\
* 3) Neutralize the supported compilers' differences.
\******************************************************************************/

// __func__ for MSVC (in MSVC 10.0, __func__ is still not supported).

#if __Y_MSVC && !defined(__func__)
	#define __func__ __FUNCTION__
#endif

// noexcept for older versions of GCC.
// MSVC doesn't support noexcept, but treats throw() just the same way.

#if ((__Y_GCC && __Y_GCC < 46) || __Y_MSVC) && !defined(noexcept)
	#define noexcept throw()
#endif

// nullptr for older versions of GCC.

#if __Y_GCC && __Y_GCC < 46 && !defined(nullptr)
	#define nullptr __null
#endif

/// \def Y_EXPORT
/// \brief Shared library exported definition specifier.

/// \def Y_IMPORT
/// \brief Shared library imported definition specifier.

/// \def Y_PRIVATE
/// \brief Shared library private definition specifier.

// GCC (thus MinGW) supports __declspec(dllexport) and __declspec(dllimport).

#if Y_IS_WINDOWS
	#define Y_EXPORT __declspec(dllexport)
	#define Y_IMPORT __declspec(dllimport)
	#define Y_PRIVATE
#else
	#if __Y_GCC
		#define Y_EXPORT __attribute__((visibility("default")))
	#else
		#define Y_EXPORT
	#endif
	#define Y_IMPORT
	#if __Y_GCC
		#define Y_PRIVATE __attribute__((visibility("hidden")))
	#else
		#define Y_PRIVATE
	#endif
#endif

/// \def Y_LIKELY
/// \brief

/// \def Y_UNLIKELY
/// \brief

#if __Y_GCC
	#define Y_LIKELY(x)   __builtin_expect(!!(x), 1)
	#define Y_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
	#define Y_LIKELY(x)   (x)
	#define Y_UNLIKELY(x) (x)
#endif

// Both MSVC and GCC support #pragma pack, so we don't bother with attributes.

/******************************************************************************\
* 4) Provide some workaroud property macros.
\******************************************************************************/

#define Y_HAVE_USER_LITERALS (__Y_GCC > 47 || __Y_MSVC > 110)

/******************************************************************************\
* 5) Provide some globally useful (now-) compiler-independent definitions.
\******************************************************************************/

/// \def Y_API
/// \brief Public API specifier.

#if defined(__YTTRIUM)
	#define Y_API Y_EXPORT
#else
	#define Y_API Y_IMPORT
#endif

/// \def Y_IS_DEBUG
/// \brief

#if defined(_DEBUG) && !defined(NDEBUG)
	#define Y_IS_DEBUG 1
#else
	#define Y_IS_DEBUG 0
#endif

/// Calculate the static \a array length.

#define Y_LENGTH_OF(array) (sizeof(array) / sizeof (array)[0])

/// %Yttrium namespace.

namespace Yttrium
{
} // namespace Yttrium

#endif // __Y_GLOBAL_HPP
