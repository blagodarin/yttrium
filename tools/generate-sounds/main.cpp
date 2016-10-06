#include <yttrium/io/file.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/static_string.h>
#include "../../src/audio/io/wav_private.h"

#include <cmath>

using namespace Yttrium;

int main(int, char**)
{
	File file("data/sound.wav", File::Write | File::Truncate);

	size_t frequency = 44100;
	size_t duration = frequency / 4; // 0.25 s.

	WavFormatChunk format_chunk;

	format_chunk.format = WAVE_FORMAT_PCM;
	format_chunk.channels = 1;
	format_chunk.samples_per_second = frequency;
	format_chunk.bits_per_sample = 16;
	format_chunk.block_align = format_chunk.channels * format_chunk.bits_per_sample / 8;
	format_chunk.bytes_per_second = format_chunk.samples_per_second * format_chunk.block_align;

	WavChunkHeader format_chunk_header;

	format_chunk_header.name_fourcc = WavChunkHeader::fmt;
	format_chunk_header.size = sizeof(format_chunk);

	WavChunkHeader data_chunk_header;

	data_chunk_header.name_fourcc = WavChunkHeader::data;
	data_chunk_header.size = duration * format_chunk.block_align;

	WavFileHeader file_header;

	file_header.riff_fourcc = WavFileHeader::RIFF;
	file_header.riff_size = sizeof(file_header.wave_fourcc) + sizeof(format_chunk_header)
		+ sizeof(format_chunk) + sizeof(data_chunk_header) + data_chunk_header.size;
	file_header.wave_fourcc = WavFileHeader::WAVE;

	if (file.write(file_header) && file.write(format_chunk_header)
		&& file.write(format_chunk) && file.write(data_chunk_header))
	{
		Buffer buffer(data_chunk_header.size);

		int16_t* data = static_cast<int16_t*>(buffer.data());

		double time_step = 440.0 / frequency;

		for (size_t i = 0; i < duration; ++i)
		{
			double base = INT16_MAX * ::sin(2 * M_PI * time_step * i);
			double amplitude = double(duration - i) / duration;
			data[i] = base * amplitude;
		}

		file.write(buffer.data(), buffer.size());
	}
}
