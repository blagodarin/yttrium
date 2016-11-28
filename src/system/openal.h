#ifndef _src_system_openal_h_
#define _src_system_openal_h_

#include "../config.h"
#if defined(Y_IS_WINDOWS) && !defined(AL_APIENTRY)
	#define AL_APIENTRY __cdecl
#endif

#include <AL/al.h>
#include <AL/alc.h>

#endif
