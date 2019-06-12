//
// Copyright 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

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
		: _frequency{ static_cast<ALsizei>(format.frames_per_second()) }
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
