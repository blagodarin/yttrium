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

#include "wav.h"

#include <yttrium/exceptions.h>
#include "../wav.h"

#include <algorithm>

namespace
{
	bool find_chunk(Yttrium::Reader& reader, uint32_t fourcc, Yttrium::WavChunkHeader& header)
	{
		for (;;)
		{
			if (!reader.read(header))
				return false;
			if (header.name_fourcc == fourcc)
				return true;
			if (!reader.skip(header.size))
				return false;
		}
	}
}

namespace Yttrium
{
	WavDecoder::WavDecoder(std::unique_ptr<Source>&& source)
		: AudioDecoder{ std::move(source) }
	{
		WavFileHeader file_header;
		if (!_reader.read(file_header)
			|| file_header.riff_fourcc != WavFileHeader::RIFF
			|| file_header.wave_fourcc != WavFileHeader::WAVE)
			throw DataError("Bad WAV header");

		WavChunkHeader fmt_header;
		WavFormatChunk fmt;
		if (!::find_chunk(_reader, WavChunkHeader::fmt, fmt_header)
			|| !_reader.read(fmt)
			|| (fmt_header.size > sizeof fmt && !_reader.skip(fmt_header.size - sizeof fmt)))
			throw DataError("Bad WAV 'fmt' chunk");

		if (fmt.format != WAVE_FORMAT_PCM)
			throw DataError("Bad WAV format (must be WAVE_FORMAT_PCM)");

		if (fmt.bits_per_sample != 16)
			throw DataError("Bad sample type");

		WavChunkHeader data_header;
		if (!::find_chunk(_reader, WavChunkHeader::data, data_header))
			throw DataError("Bad WAV 'data' chunk");

		_format = { AudioSample::i16, fmt.channels, fmt.samples_per_second };
		_total_frames = std::min<uint64_t>(_reader.size() - _reader.offset(), data_header.size) / _format.bytes_per_frame();
		_data_offset = _reader.offset();
	}

	size_t WavDecoder::read(void* buffer, size_t bytes_to_read)
	{
		const auto frame_bytes = _format.bytes_per_frame();
		bytes_to_read = static_cast<size_t>(std::min<uint64_t>(bytes_to_read / frame_bytes, _total_frames - _current_frame)) * frame_bytes;
		const auto bytes_read = _reader.read(buffer, bytes_to_read);
		_current_frame += bytes_read / frame_bytes;
		return bytes_read;
	}

	bool WavDecoder::seek(uint64_t offset)
	{
		if (offset > _total_frames)
			return false;
		if (!_reader.seek(_data_offset + offset * _format.bytes_per_frame()))
			return false;
		_current_frame = offset;
		return true;
	}
}
