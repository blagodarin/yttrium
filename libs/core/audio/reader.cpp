//
// Copyright 2018 Sergei Blagodarin
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

#include "reader.h"

#include <yttrium/exceptions.h>
#include "../../core/utils/fourcc.h"
#include "formats/wav.h"

#if Y_USE_OGG_VORBIS
#	include "formats/ogg_vorbis.h"
#endif

namespace
{
	enum class AudioType
	{
		Unknown,
		Wav,
#if Y_USE_OGG_VORBIS
		OggVorbis,
#endif
	};

	AudioType detect_audio_type(const Yttrium::Source& source)
	{
		using namespace Yttrium::Literals;

		uint32_t signature = 0;
		if (!source.read_at(0, signature))
			return AudioType::Unknown;
		switch (signature)
		{
		case "RIFF"_fourcc: return AudioType::Wav;
#if Y_USE_OGG_VORBIS
		case "OggS"_fourcc: return AudioType::OggVorbis;
#endif
		default: return AudioType::Unknown;
		}
	}
}

namespace Yttrium
{
	std::unique_ptr<AudioReader> AudioReader::open(std::unique_ptr<Source>&& source)
	{
		if (!source)
			return {};
		switch (::detect_audio_type(*source))
		{
		case AudioType::Wav: return std::make_unique<WavReader>(std::move(source));
#if Y_USE_OGG_VORBIS
		case AudioType::OggVorbis: return std::make_unique<OggVorbisReader>(std::move(source));
#endif
		default: throw DataError{ "Unknown audio format" };
		}
	}
}
