/// \file
/// \brief Global definitions.

#ifndef _include_yttrium_global_h_
#define _include_yttrium_global_h_

/// \def __YTTRIUM_SHARED
/// \brief %Yttrium shared library compilation "flag".
/// \note Must be defined when building %Yttrium as a shared library.

/// \def __YTTRIUM_STATIC
/// \brief %Yttrium static library compilation "flag".
/// \note Must be defined when building %Yttrium as a static library
/// **or using it as one**.

#if defined(__YTTRIUM_DOXYGEN)
	#define __YTTRIUM_SHARED
	#define __YTTRIUM_STATIC
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Compiler detection.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// \def Y_COMPILER_GCC
/// \brief GCC version number (e.g. 47 for GCC 4.7), or 0 if it's not GCC.
#if defined(__GNUC__)
	#define Y_COMPILER_GCC (__GNUC__ * 10 + __GNUC_MINOR__)
	#if Y_COMPILER_GCC < 47
		#error GCC compilers older than 4.7 are not supported.
	#endif
	#define __Y_HAS_COMPILER
#else
	#define Y_COMPILER_GCC 0
#endif

/// \def Y_COMPILER_MSVC
/// \brief MSVC version number (e.g. 71 for MSVC 7.1, 100 for MSVC 10.0).
#if defined(_MSC_VER)
	#define Y_COMPILER_MSVC (_MSC_VER / 10 - 60)
	#if Y_COMPILER_MSVC < 120
		#error MSVC compilers older than 12.0 are not supported.
	#endif
	#define __Y_HAS_COMPILER
#else
	#define Y_COMPILER_MSVC 0
#endif

#ifndef __Y_HAS_COMPILER
	#error Only the GCC and MSVC compilers are supported.
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Platform detection.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// \def Y_PLATFORM_LINUX
/// \brief Defined to 1 when compiling for Linux.
#if defined(__linux__)
	#define Y_PLATFORM_LINUX 1
	#define __Y_HAS_PLATFORM
#else
	#define Y_PLATFORM_LINUX 0
#endif

/// \def Y_PLATFORM_POSIX
/// \brief Defined to 1 when compiling for a POSIX system, including Linux.
#if Y_PLATFORM_LINUX || defined(__unix__)
	#define Y_PLATFORM_POSIX 1
	#define __Y_HAS_PLATFORM
#else
	#define Y_PLATFORM_POSIX 0
#endif

/// \def Y_PLATFORM_WINDOWS
/// \brief Defined to 1 when compiling for Windows.
#if defined(_WIN32) || defined(__CYGWIN__)
	#define Y_PLATFORM_WINDOWS 1
	#define __Y_HAS_PLATFORM
#else
	#define Y_PLATFORM_WINDOWS 0
#endif

#ifndef __Y_HAS_PLATFORM
	#error Unknown target platform.
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Compiler portability.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// \def Y_EXPORT
/// \brief Shared library exported declaration specifier.
#if Y_PLATFORM_WINDOWS
	#define Y_EXPORT __declspec(dllexport)
#elif Y_COMPILER_GCC
	#define Y_EXPORT __attribute__((visibility("default")))
#else
	#define Y_EXPORT
#endif

/// \def Y_IMPORT
/// \brief Shared library imported declaration specifier.
#if Y_PLATFORM_WINDOWS
	#define Y_IMPORT __declspec(dllimport)
#else
	#define Y_IMPORT
#endif

/// \def Y_PRIVATE
/// \brief Shared library private declaration specifier.
#if Y_PLATFORM_WINDOWS
	#define Y_PRIVATE
#elif Y_COMPILER_GCC
	#define Y_PRIVATE __attribute__((visibility("hidden")))
#else
	#define Y_PRIVATE
#endif

/// Non-returning function attribute.
#if Y_COMPILER_GCC
	#define Y_NORETURN __attribute__((__noreturn__))
#elif Y_COMPILER_MSVC
	#define Y_NORETURN __declspec(noreturn)
#else
	#define Y_NORETURN
#endif

// Both MSVC and GCC support #pragma pack, so we don't bother with attributes.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Compiler-independent declarations.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Public %Yttrium API specifier.

#if !defined(__YTTRIUM_DOXYGEN)
	#if defined(__YTTRIUM_SHARED)
		#define Y_API Y_EXPORT
	#elif !defined(__YTTRIUM_STATIC)
		#define Y_API Y_IMPORT
	#endif
#endif

#if !defined(Y_API)
	#define Y_API
#endif

/// Defined to 1 when compiling in the debug mode.

#if defined(_DEBUG) && !defined(NDEBUG)
	#define Y_IS_DEBUG 1
#else
	#define Y_IS_DEBUG 0
#endif

/// \def Y_UNUSED
/// \brief Mark the specified parameter as unused.
/// \note Don't use this macro to mark the parameters that are never going
/// to be used, prefer the parameter name omission in that case instead.

#define Y_UNUSED(parameter) (void)(parameter)

/// %Yttrium namespace.
namespace Yttrium
{
}

#endif
