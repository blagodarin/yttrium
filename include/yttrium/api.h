#pragma once

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

#if defined(YTTRIUM_EXPORT)
	#define Y_API Y_EXPORT
#elif defined(YTTRIUM_IMPORT)
	#define Y_API Y_IMPORT
#else
	#define Y_API
#endif
