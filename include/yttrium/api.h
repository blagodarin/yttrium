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

#if defined(Y_CORE_EXPORT)
	#define Y_CORE_API Y_EXPORT
#elif defined(Y_CORE_IMPORT)
	#define Y_CORE_API Y_IMPORT
#else
	#define Y_CORE_API
#endif

#if defined(Y_ENGINE_EXPORT)
	#define Y_ENGINE_API Y_EXPORT
#elif defined(Y_ENGINE_IMPORT)
	#define Y_ENGINE_API Y_IMPORT
#else
	#define Y_ENGINE_API
#endif
