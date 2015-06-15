#ifndef __AUDIO_BACKEND_OPENAL_FORMAT_H
#define __AUDIO_BACKEND_OPENAL_FORMAT_H

#include <yttrium/audio/io.h>
#include "openal.h"

namespace Yttrium
{
	class AudioFormat;

	class OpenAlFormat
	{
	public:

		ALuint _format = 0;
		ALsizei _frequency = 0;

		bool set(const AudioFormat& format);
	};
}

#endif // __AUDIO_BACKEND_OPENAL_FORMAT_H
