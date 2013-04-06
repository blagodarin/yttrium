/// \file
/// \brief Global definitions.
/// \note This header is included (directly or indirectly) by all other %Yttrium headers.

// All the macros that start with __Y_ are internal to a single header and should
// only be used in that header. As such, they are pseudo-documented by the ordinary
// comments to provide some information about them, but hide them from the doxygen.

#ifndef __Y_GLOBAL_H
#define __Y_GLOBAL_H

/// \def __YTTRIUM_SHARED
/// \brief Yttrium shared library compilation "flag".
/// \note Must be defined when building %Yttrium as a shared library.

/// \def __YTTRIUM_STATIC
/// \brief Yttrium static library compilation "flag".
/// \note Must be defined when building %Yttrium as a static library <strong>or using it as one</strong>.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Compilers.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// \def __Y_GCC
// \brief GCC version number (e.g. 47 for GCC 4.7).

#if defined(__GNUC__)
	#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)
		#define __Y_GCC (__GNUC__ * 10 + __GNUC_MINOR__)
	#else
		#error GCC compilers older than 4.7 are not supported.
	#endif
#else
	#define __Y_GCC 0
#endif

// \def __Y_MSVC
// \brief MSVC version number (e.g. 71 for MSVC 7.1, 100 for MSVC 10.0).

#if defined(_MSC_VER)
	#if _MSC_VER >= 1700
		#define __Y_MSVC 110
	#else
		#error MSVC compilers older than 11.0 are not supported.
	#endif
#else
	#define __Y_MSVC 0
#endif

// The above compilers should suffice.

#if !(__Y_GCC || __Y_MSVC)
	#error Only the GCC and MSVC compilers are supported.
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Target platforms.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// \def Y_IS_LINUX
/// \brief Defined to 1 when compiling for Linux.

#if defined(__linux__)
	#define Y_IS_LINUX 1
#else
	#define Y_IS_LINUX 0
#endif

/// \def Y_IS_POSIX
/// \brief Defined to 1 when compiling for a POSIX system, including Linux.

#if Y_IS_LINUX || defined(__unix__)
	#define Y_IS_POSIX 1
#else
	#define Y_IS_POSIX 0
#endif

/// \def Y_IS_WINDOWS
/// \brief Defined to 1 when compiling for Windows.

#if defined(_WIN32) || defined(__CYGWIN__)
	#define Y_IS_WINDOWS 1
#else
	#define Y_IS_WINDOWS 0
#endif

// Check the correctness of the platform definitions.

#if Y_IS_POSIX && Y_IS_WINDOWS
	#error Invalid target platform.
#endif

#if !(Y_IS_POSIX || Y_IS_WINDOWS)
	#error Unknown target platform.
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cross-compiler portability.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// __func__ for MSVC (in MSVC 11.0, __func__ is still unsupported).

#if __Y_MSVC && !defined(__func__)
	#define __func__ __FUNCTION__
#endif

// MSVC doesn't support noexcept, but treats throw() just the same way.

#if __Y_MSVC && !defined(noexcept)
	#define noexcept throw()
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Compiler-independent definitions.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// \def Y_API
/// \brief Public %Yttrium API specifier.

#if defined(__YTTRIUM_SHARED)
	#define Y_API Y_EXPORT
#else
	#define Y_API Y_IMPORT
#endif

#if !defined(Y_API)
	#define Y_API
#endif

/// \def Y_IS_DEBUG
/// \brief Defined to 1 when compiling in the debug mode.

#if defined(_DEBUG) && !defined(NDEBUG)
	#define Y_IS_DEBUG 1
#else
	#define Y_IS_DEBUG 0
#endif

/// Calculate the static \a array length.

#define Y_LENGTH_OF(array) (sizeof(array) / sizeof (array)[0])

/// Make the current class \a Class non-copyable.

#define Y_NONCOPYABLE(Class) \
	Class(const Class &) = delete; \
	Class &operator =(const Class &) = delete

/// Mark the specified parameter as unused.
/// \note Don't use this macro to mark the parameters that are never planned
/// to be used, prefer the parameter name omission in that case instead.

#define Y_UNUSED(parameter) (void)(parameter)

/// %Yttrium namespace.

namespace Yttrium
{

} // namespace Yttrium

#endif // __Y_GLOBAL_H
