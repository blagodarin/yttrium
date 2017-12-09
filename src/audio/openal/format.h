#ifndef _src_audio_openal_format_h_
#define _src_audio_openal_format_h_

#include "../../platform/openal.h"

#include <cstddef>

namespace Yttrium
{
	class OpenALFormat
	{
	public:
		ALenum _format = 0;
		ALsizei _frequency = 0;

		OpenALFormat() = default;
		explicit OpenALFormat(const class AudioFormat&);
	};
}

#endif
