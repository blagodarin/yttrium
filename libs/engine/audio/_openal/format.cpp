#include "format.h"

#include <yttrium/audio/format.h>
#include <yttrium/exceptions.h>

#include <algorithm>
#include <vector>

// OpenAL doesn't expose its limitations, if any, so a list of common sampling rates should do.
const std::vector<ALsizei> _common_sampling_rates{ 8'000, 11'025, 16'000, 22'050, 32'000, 44'100, 48'000, 88'200, 96'000 };

namespace Yttrium
{
	OpenALFormat::OpenALFormat(const AudioFormat& format)
		: _frequency{ static_cast<ALsizei>(format.samples_per_second()) }
	{
		switch (format.bytes_per_sample())
		{
		case 1:
			switch (format.channels())
			{
			case 1: _format = AL_FORMAT_MONO8; break;
			case 2: _format = AL_FORMAT_STEREO8; break;
			default: throw NotSupportedError{ "OpenAL backend doesn't support ", format.channels(), "-channel audio" };
			}
			break;
		case 2:
			switch (format.channels())
			{
			case 1: _format = AL_FORMAT_MONO16; break;
			case 2: _format = AL_FORMAT_STEREO16; break;
			default: throw NotSupportedError{ "OpenAL backend doesn't support ", format.channels(), "-channel audio" };
			}
			break;
		default:
			throw NotSupportedError{ "OpenAL backend doesn't support ", format.bytes_per_sample(), " bytes per sample" };
		}
		if (std::find(_common_sampling_rates.begin(), _common_sampling_rates.end(), _frequency) == _common_sampling_rates.end())
			throw NotSupportedError{ "Unexpected sampling rate: ", _frequency, " Hz" };
	}
}
