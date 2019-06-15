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

#include <yttrium/audio/reader.h>

#include <yttrium/exceptions.h>
#include "../../core/utils/fourcc.h"
#include "formats/wav.h"

#if Y_USE_OGG_VORBIS
#	include "formats/ogg_vorbis.h"
#endif

namespace
{
	std::unique_ptr<Yttrium::AudioDecoder> create_audio_decoder(std::unique_ptr<Yttrium::Source>&& source)
	{
		using namespace Yttrium::Literals;
		if (!source)
			throw std::logic_error{ "Can't create AudioDecoder from an empty Source" };
		uint32_t signature = 0;
		if (source->read_at(0, signature))
		{
			switch (signature)
			{
			case "RIFF"_fourcc: return std::make_unique<Yttrium::WavDecoder>(std::move(source));
#if Y_USE_OGG_VORBIS
			case "OggS"_fourcc: return std::make_unique<Yttrium::OggVorbisDecoder>(std::move(source));
#endif
			}
		}
		throw Yttrium::DataError{ "Unknown audio format" };
	}
}

namespace Yttrium
{
	AudioReader::AudioReader(std::unique_ptr<Source>&& source)
		: _decoder{ ::create_audio_decoder(std::move(source)) } {}

	AudioReader::~AudioReader() noexcept = default;

	uint64_t AudioReader::current_frame() const noexcept
	{
		return _decoder->current_frame();
	}

	AudioFormat AudioReader::format() const noexcept
	{
		return _decoder->format();
	}

	size_t AudioReader::read(void* data, size_t bytes)
	{
		return _decoder->read(data, bytes);
	}

	bool AudioReader::seek(uint64_t frame_offset)
	{
		return _decoder->seek(frame_offset);
	}

	uint64_t AudioReader::total_bytes() const noexcept
	{
		return _decoder->total_bytes();
	}

	uint64_t AudioReader::total_frames() const noexcept
	{
		return _decoder->total_frames();
	}
}
