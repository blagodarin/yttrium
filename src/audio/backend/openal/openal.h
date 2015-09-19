#ifndef _src_audio_backend_openal_openal_h_
#define _src_audio_backend_openal_openal_h_

#include <yttrium/global.h>

#if Y_PLATFORM_WINDOWS && !defined(AL_APIENTRY)
	#define AL_APIENTRY __cdecl
#endif

#include <AL/al.h>
#include <AL/alc.h>

#endif
