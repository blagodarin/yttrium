#ifndef _src_audio_openal_format_h_
#define _src_audio_openal_format_h_

#include "../../system/openal.h"

namespace Yttrium
{
	class OpenALFormat
	{
	public:
		ALuint _format = 0;
		ALsizei _frequency = 0;

		OpenALFormat() = default;
		explicit OpenALFormat(const class AudioFormat&);
	};
}

#endif
