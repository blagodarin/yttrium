/// \file
/// \brief API definitions.

#ifndef _include_yttrium_api_h_
#define _include_yttrium_api_h_

/// \def Y_EXPORT
/// \brief Exported API specifier.

/// \def Y_IMPORT
/// \brief Imported API specifier.

/// \def Y_PRIVATE
/// \brief Prevents a declaration from being exported as a part of API.

#if defined(_WIN32) || defined(__CYGWIN__)
	#define Y_EXPORT __declspec(dllexport)
	#define Y_IMPORT __declspec(dllimport)
	#define Y_PRIVATE
#elif defined(__GNUC__)
	#define Y_EXPORT __attribute__((visibility("default")))
	#define Y_IMPORT
	#define Y_PRIVATE __attribute__((visibility("hidden")))
#else
	#error
#endif

/// \def Y_API
/// \brief %Yttrium library API specifier.

#ifdef YTTRIUM_EXPORT
	#define Y_API Y_EXPORT
#else
	#define Y_API Y_IMPORT
#endif

#endif
