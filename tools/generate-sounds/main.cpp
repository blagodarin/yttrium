#include <yttrium/memory/buffer.h>
#include <yttrium/storage/writer.h>
#include "../../src/audio/formats/wav_private.h"

#include <cmath>
#include <limits>

using namespace Yttrium;

int main()
{
	Writer writer("data/sound.wav");

	constexpr uint32_t frequency = 44100;
	constexpr uint32_t duration = frequency / 4; // 0.25 s.

	WavFormatChunk format_chunk;
	format_chunk.format = WAVE_FORMAT_PCM;
	format_chunk.channels = 1;
	format_chunk.samples_per_second = frequency;
	format_chunk.bits_per_sample = 16;
	format_chunk.block_align = static_cast<uint16_t>(format_chunk.channels * format_chunk.bits_per_sample / 8);
	format_chunk.bytes_per_second = format_chunk.samples_per_second * format_chunk.block_align;

	WavChunkHeader format_chunk_header;
	format_chunk_header.name_fourcc = WavChunkHeader::fmt;
	format_chunk_header.size = sizeof(format_chunk);

	WavChunkHeader data_chunk_header;
	data_chunk_header.name_fourcc = WavChunkHeader::data;
	data_chunk_header.size = duration * format_chunk.block_align;

	WavFileHeader file_header;
	file_header.riff_fourcc = WavFileHeader::RIFF;
	file_header.riff_size = static_cast<uint32_t>(sizeof file_header.wave_fourcc + sizeof format_chunk_header + sizeof format_chunk + sizeof data_chunk_header + data_chunk_header.size);
	file_header.wave_fourcc = WavFileHeader::WAVE;

	if (writer.write(file_header) && writer.write(format_chunk_header) && writer.write(format_chunk) && writer.write(data_chunk_header))
	{
		Buffer buffer(data_chunk_header.size);

		const auto data = static_cast<int16_t*>(buffer.data());

		constexpr auto time_step = 440.0 / frequency;

		for (uint32_t i = 0; i < duration; ++i)
		{
			const auto base = std::numeric_limits<int16_t>::max() * std::sin(2 * M_PI * time_step * i);
			const auto amplitude = static_cast<double>(duration - i) / duration;
			data[i] = static_cast<int16_t>(base * amplitude);
		}

		writer.write_all(buffer);
	}
}
