#ifndef _src_audio_backend_openal_format_h_
#define _src_audio_backend_openal_format_h_

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

#endif
