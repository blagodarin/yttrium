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

#include "reader.h"

#include <yttrium/exceptions.h>
#include "../../core/utils/fourcc.h"
#include "formats/wav.h"

#if Y_USE_OGG_VORBIS
#	include "formats/ogg_vorbis.h"
#endif

namespace Yttrium
{
	std::unique_ptr<AudioReader> AudioReader::open(std::unique_ptr<Source>&& source)
	{
		if (!source)
			return {};
		uint32_t signature = 0;
		if (source->read_at(0, signature))
		{
			switch (signature)
			{
				case "RIFF"_fourcc: return std::make_unique<AudioReaderImpl>(std::make_unique<WavDecoder>(std::move(source)));
#if Y_USE_OGG_VORBIS
				case "OggS"_fourcc: return std::make_unique<AudioReaderImpl>(std::make_unique<OggVorbisDecoder>(std::move(source)));
#endif
			}
		}
		throw DataError{ "Unknown audio format" };
	}
}
