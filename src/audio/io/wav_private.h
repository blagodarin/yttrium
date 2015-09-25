#ifndef _src_audio_io_wav_private_h_
#define _src_audio_io_wav_private_h_

#include "../../base/fourcc.h"

namespace Yttrium
{
	enum: uint16_t
	{
		WAVE_FORMAT_PCM = 0x0001,
	};

#pragma pack(push, 1)

	struct WavFileHeader
	{
		enum: uint32_t
		{
			RIFF = "RIFF"_fourcc,
			WAVE = "WAVE"_fourcc,
		};

		uint32_t riff_fourcc;
		uint32_t riff_size;
		uint32_t wave_fourcc;
	};

	struct WavChunkHeader
	{
		enum: uint32_t
		{
			fmt = "fmt "_fourcc,
			data = "data"_fourcc,
		};

		uint32_t name_fourcc;
		uint32_t size;
	};

	struct WavFormatChunk
	{
		uint16_t format;
		uint16_t channels;
		uint32_t samples_per_second;
		uint32_t bytes_per_second;
		uint16_t block_align;
		uint16_t bits_per_sample;
	};

#pragma pack(pop)
}

#endif
