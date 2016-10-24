#include "format.h"

#include <yttrium/audio/io.h>
#include <yttrium/exceptions.h>

namespace Yttrium
{
	OpenALFormat::OpenALFormat(const AudioFormat& format)
		: _frequency(format.frequency) // TODO: Check the frequency too.
	{
		switch (format.bytes_per_sample)
		{
		case 1:
			switch (format.channels)
			{
			case 1: _format = AL_FORMAT_MONO8; break;
			case 2: _format = AL_FORMAT_STEREO8; break;
			default: throw DataError("OpenAL backend doesn't support ", format.channels, "-channel audio");
			}
			break;
		case 2:
			switch (format.channels)
			{
			case 1: _format = AL_FORMAT_MONO16; break;
			case 2: _format = AL_FORMAT_STEREO16; break;
			default: throw DataError("OpenAL backend doesn't support ", format.channels, "-channel audio");
			}
			break;
		default:
			throw DataError("OpenAL backend doesn't support ", format.bytes_per_sample, " bytes per sample");
		}
	}

	// TODO: Change exception type.
}
