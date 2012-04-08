#ifndef __AUDIO_OPENAL_OPENAL_HPP
#define __AUDIO_OPENAL_OPENAL_HPP

#include <Yttrium/global.hpp>

#if Y_IS_WINDOWS && !defined(AL_APIENTRY)
#	define AL_APIENTRY __cdecl
#endif

#include <AL/al.h>
#include <AL/alc.h>

#endif // __AUDIO_OPENAL_OPENAL_HPP
