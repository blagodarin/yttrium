#ifndef _include_yttrium_audio_utils_h_
#define _include_yttrium_audio_utils_h_

#include <yttrium/api.h>

#include <cstddef>

namespace Yttrium
{
	class AudioFormat;
	class Writer;

	///
	Y_API bool write_wav_header(Writer&, const AudioFormat&, size_t samples);
}

#endif
