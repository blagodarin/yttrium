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
#include <yttrium/utils/numeric.h>
#include "../../core/utils/fourcc.h"
#include "formats/wav.h"

#if Y_USE_OGG_VORBIS
#	include "formats/ogg_vorbis.h"
#endif

#include <algorithm>
#include <cassert>

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
		: _decoder{ ::create_audio_decoder(std::move(source)) }
		, _end_frame{ _decoder->total_frames() }
		, _loop_frame{ _end_frame }
	{
	}

	AudioReader::~AudioReader() noexcept = default;

	AudioFormat AudioReader::format() const noexcept
	{
		return _decoder->format();
	}

	size_t AudioReader::read(void* data, size_t bytes_to_read)
	{
		const auto frame_size = _decoder->format().frame_bytes();
		for (size_t result = 0;;)
		{
			const auto requested = bytes_to_read - result;
			const auto available = (_end_frame - _decoder->current_frame()) * frame_size;
			const auto decoded = _decoder->read(static_cast<std::byte*>(data) + result, static_cast<size_t>(std::min<uint64_t>(requested, available)));
			result += decoded;
			if (decoded == requested || decoded != available || _loop_frame >= _end_frame || !_decoder->seek(_loop_frame))
				return result;
		}
	}

	bool AudioReader::seek(uint64_t frame_offset)
	{
		return frame_offset < _end_frame && _decoder->seek(frame_offset);
	}

	bool AudioReader::set_loop(std::chrono::milliseconds from, std::chrono::milliseconds to) noexcept
	{
		if (from < std::chrono::milliseconds{ 0 } || from >= to)
			return false;
		const auto total_frames = _decoder->total_frames();
		const auto frames_per_second = uint64_t{ _decoder->format().frames_per_second() };
		const auto end_time = std::chrono::milliseconds{ (total_frames * 1000 + frames_per_second - 1) / frames_per_second };
		if (from >= end_time)
			return false;
		if (to == std::chrono::milliseconds::max())
			to = end_time;
		else if (to > end_time)
			return false;
		const auto end_frame = std::min(to_unsigned(to.count()) * frames_per_second / 1000, total_frames);
		if (end_frame < _decoder->current_frame())
			return false;
		_loop_frame = to_unsigned(from.count()) * frames_per_second / 1000;
		assert(_loop_frame < total_frames);
		_end_frame = end_frame;
		return true;
	}

	uint64_t AudioReader::total_bytes() const noexcept
	{
		return _decoder->total_bytes();
	}
}
