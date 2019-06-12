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

#include <yttrium/audio/utils.h>

#include <yttrium/audio/format.h>
#include <yttrium/storage/writer.h>
#include "formats/wav_private.h"

#include <limits>

namespace
{
	const size_t _max_wav_data_size = std::numeric_limits<int32_t>::max()
		- sizeof(Yttrium::WavFileHeader)
		- sizeof(Yttrium::WavChunkHeader) - sizeof(Yttrium::WavFormatChunk)
		- sizeof(Yttrium::WavChunkHeader);
}

namespace Yttrium
{
	bool write_wav_header(Writer& writer, const AudioFormat& format, size_t samples)
	{
		if (!(format.bytes_per_sample() == 1 || format.bytes_per_sample() == 2)
			|| !(format.channels() == 1 || format.channels() == 2)
			|| !(format.frames_per_second() >= 8000 && format.frames_per_second() <= 192'000)
			|| samples > ::_max_wav_data_size / format.bytes_per_sample())
			return false;

		WavFormatChunk format_chunk;
		format_chunk.format = WAVE_FORMAT_PCM;
		format_chunk.channels = static_cast<uint16_t>(format.channels());
		format_chunk.samples_per_second = static_cast<uint32_t>(format.frames_per_second());
		format_chunk.bits_per_sample = static_cast<uint16_t>(format.bytes_per_sample() * 8);
		format_chunk.block_align = static_cast<uint16_t>(format_chunk.channels * format_chunk.bits_per_sample / 8);
		format_chunk.bytes_per_second = format_chunk.samples_per_second * format_chunk.block_align;

		WavChunkHeader format_chunk_header;
		format_chunk_header.name_fourcc = WavChunkHeader::fmt;
		format_chunk_header.size = sizeof format_chunk;

		WavChunkHeader data_chunk_header;
		data_chunk_header.name_fourcc = WavChunkHeader::data;
		data_chunk_header.size = static_cast<uint32_t>(samples * format_chunk.block_align);

		WavFileHeader file_header;
		file_header.riff_fourcc = WavFileHeader::RIFF;
		file_header.riff_size = static_cast<uint32_t>(sizeof file_header.wave_fourcc + sizeof format_chunk_header + sizeof format_chunk + sizeof data_chunk_header + data_chunk_header.size);
		file_header.wave_fourcc = WavFileHeader::WAVE;

		return writer.write(file_header) && writer.write(format_chunk_header) && writer.write(format_chunk) && writer.write(data_chunk_header);
	}
}
