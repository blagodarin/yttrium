// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "wav.h"

#include <yttrium/base/exceptions.h>
#include "../wav.h"

#include <algorithm>

namespace
{
	bool find_chunk(Yt::Reader& reader, uint32_t fourcc, Yt::WavChunkHeader& header)
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

namespace Yt
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

		AudioSample sample_type;
		if (fmt.format == WAVE_FORMAT_PCM && fmt.bits_per_sample == 16)
			sample_type = AudioSample::i16;
		else if (fmt.format == WAVE_FORMAT_IEEE_FLOAT && fmt.bits_per_sample == 32)
			sample_type = AudioSample::f32;
		else
			throw DataError("Unsupported WAV sample type");

		WavChunkHeader data_header;
		if (!::find_chunk(_reader, WavChunkHeader::data, data_header))
			throw DataError("Bad WAV 'data' chunk");

		_format = { sample_type, fmt.channels, fmt.samples_per_second };
		_total_frames = std::min<uint64_t>(_reader.size() - _reader.offset(), data_header.size) / _format.bytes_per_frame();
		_data_offset = _reader.offset();
	}

	size_t WavDecoder::read_frames(void* buffer, size_t frames)
	{
		const auto bytes_per_frame = _format.bytes_per_frame();
		const auto bytes_to_read = static_cast<size_t>(std::min<uint64_t>(frames, _total_frames - _current_frame)) * bytes_per_frame;
		const auto result = _reader.read(buffer, bytes_to_read) / bytes_per_frame;
		_current_frame += result;
		return result;
	}

	bool WavDecoder::seek_frame(uint64_t frame)
	{
		if (frame > _total_frames)
			return false;
		if (!_reader.seek(_data_offset + frame * _format.bytes_per_frame()))
			return false;
		_current_frame = frame;
		return true;
	}
}
