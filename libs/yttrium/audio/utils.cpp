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
			|| !(format.samples_per_second() >= 8000 && format.samples_per_second() <= 192'000)
			|| samples > ::_max_wav_data_size / format.bytes_per_sample())
			return false;

		WavFormatChunk format_chunk;
		format_chunk.format = WAVE_FORMAT_PCM;
		format_chunk.channels = static_cast<uint16_t>(format.channels());
		format_chunk.samples_per_second = static_cast<uint32_t>(format.samples_per_second());
		format_chunk.bits_per_sample = static_cast<uint16_t>(format.bytes_per_sample() * 8);
		format_chunk.block_align = static_cast<uint16_t>(format_chunk.channels * format_chunk.bits_per_sample / 8);
		format_chunk.bytes_per_second = static_cast<uint32_t>(format_chunk.samples_per_second * format_chunk.block_align);

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
