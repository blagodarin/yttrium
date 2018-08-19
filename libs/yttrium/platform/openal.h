#ifndef _src_platform_openal_h_
#define _src_platform_openal_h_

#if defined(_WIN32) && !defined(AL_APIENTRY)
	#define AL_APIENTRY __cdecl
#endif

#include <al.h>
#include <alc.h>

#endif
