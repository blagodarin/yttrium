#ifndef __AUDIO_BACKEND_OPENAL_OPENAL_H
#define __AUDIO_BACKEND_OPENAL_OPENAL_H

#include <yttrium/global.h>

#if Y_PLATFORM_WINDOWS && !defined(AL_APIENTRY)
	#define AL_APIENTRY __cdecl
#endif

#include <AL/al.h>
#include <AL/alc.h>

#endif // __AUDIO_BACKEND_OPENAL_OPENAL_H
