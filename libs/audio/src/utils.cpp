// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/audio/utils.h>

#include <yttrium/audio/format.h>
#include <yttrium/storage/writer.h>
#include "processing.h"
#include "wav.h"

#include <cstring>
#include <limits>

namespace Yt
{
	bool transform_audio(void* dst, const AudioFormat& dst_format, const void* src, const AudioFormat& src_format, size_t frames)
	{
		if (dst_format.frames_per_second() != src_format.frames_per_second())
			return false;

		const auto dst_sample = dst_format.sample_type();
		const auto src_sample = src_format.sample_type();

		const auto dst_channels = dst_format.channels();
		const auto src_channels = src_format.channels();

		if (dst_sample == src_sample && dst_channels == src_channels)
		{
			std::memcpy(dst, src, frames * src_format.bytes_per_frame());
			return true;
		}

		switch (dst_sample)
		{
		case AudioSample::i16:
			if (src_sample == AudioSample::i16 && dst_channels == 2 && src_channels == 1)
			{
				duplicate_i16(dst, src, frames);
				return true;
			}
			break;

		case AudioSample::f32:
			switch (src_sample)
			{
			case AudioSample::i16:
				if (dst_channels == src_channels)
				{
					convert_normalize_f32_i16(dst, src, frames * src_channels);
					return true;
				}
				else if (dst_channels == 2 && src_channels == 1)
				{
					convert_normalize_duplicate_f32_i16(dst, src, frames);
					return true;
				}
				break;

			case AudioSample::f32:
				if (dst_channels == 2 && src_channels == 1)
				{
					duplicate_f32(dst, src, frames);
					return true;
				}
				break;
			}
			break;
		}

		return false;
	}

	bool write_wav_header(Writer& writer, const AudioFormat& format, size_t samples)
	{
		constexpr size_t max_wav_data_size = std::numeric_limits<int32_t>::max()
			- sizeof(WavFileHeader)
			- sizeof(WavChunkHeader) - sizeof(WavFormatChunk)
			- sizeof(WavChunkHeader);

		if (!(format.channels() == 1 || format.channels() == 2)
			|| !(format.frames_per_second() >= 8000 && format.frames_per_second() <= 192'000)
			|| samples > max_wav_data_size / format.bytes_per_sample())
			return false;

		WavFormatChunk format_chunk;
		switch (format.sample_type())
		{
		case AudioSample::i16: format_chunk.format = WAVE_FORMAT_PCM; break;
		case AudioSample::f32: format_chunk.format = WAVE_FORMAT_IEEE_FLOAT; break;
		default: return false;
		}
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
