// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "ogg_vorbis.h"

#include <yttrium/base/exceptions.h>
#include <yttrium/base/numeric.h>

#include <algorithm>
#include <cstring>

namespace
{
	size_t read_callback(void* ptr, size_t size, size_t nmemb, void* datasource)
	{
		return static_cast<Yt::Reader*>(datasource)->read(ptr, size * nmemb);
	}

	int seek_callback(void* datasource, ogg_int64_t offset, int whence)
	{
		auto& reader = *static_cast<Yt::Reader*>(datasource);
		switch (whence)
		{
		case SEEK_SET: return offset >= 0 && reader.seek(Yt::to_unsigned(offset)) ? 0 : -1;
		case SEEK_CUR: return offset >= 0 && reader.skip(Yt::to_unsigned(offset)) ? 0 : -1;
		case SEEK_END: return offset == 0 && reader.seek(reader.size()) ? 0 : -1;
		default: return -1;
		}
	}

	int close_callback(void*)
	{
		return 0;
	}

	long tell_callback(void* datasource)
	{
		return static_cast<long>(static_cast<Yt::Reader*>(datasource)->offset());
	}

	const ov_callbacks _ov_callbacks{
		read_callback,
		seek_callback,
		close_callback,
		tell_callback,
	};
}

namespace Yt
{
	OggVorbisDecoder::OggVorbisDecoder(std::unique_ptr<Source>&& source)
		: AudioDecoder{ std::move(source) }
	{
		::memset(&_ov_file, 0, sizeof _ov_file);
		if (::ov_open_callbacks(&_reader, &_ov_file, nullptr, 0, ::_ov_callbacks) < 0)
			throw DataError("Bad Ogg Vorbis file");

		vorbis_info* info = ::ov_info(&_ov_file, -1);
		if (info->channels <= 0 || info->rate <= 0)
			throw DataError("Bad Ogg Vorbis file");

		const auto total_frames = ::ov_pcm_total(&_ov_file, -1);
		if (total_frames < 0)
			throw DataError("Bad Ogg Vorbis file");

		_format = AudioFormat{ AudioSample::i16, to_unsigned(info->channels), to_unsigned(info->rate) };
		_total_frames = to_unsigned(total_frames);
	}

	OggVorbisDecoder::~OggVorbisDecoder()
	{
		::ov_clear(&_ov_file);
	}

	size_t OggVorbisDecoder::read_frames(void* buffer, size_t frames)
	{
		const auto bytes_per_frame = _format.bytes_per_frame();
		const auto bytes_to_read = static_cast<size_t>(std::min<uint64_t>(frames, _total_frames - _current_frame)) * bytes_per_frame;
		size_t bytes_read = 0;
		for (int bitstream = 0; bytes_read <= bytes_to_read;)
		{
			const auto size = static_cast<int>(std::min<size_t>(bytes_to_read - bytes_read, to_unsigned(std::numeric_limits<int>::max())));
			const auto read = ::ov_read(&_ov_file, static_cast<char*>(buffer) + bytes_read, size, 0, 2, 1, &bitstream);
			if (read <= 0)
				break;
			bytes_read += to_unsigned(read);
		}
		const auto frames_read = bytes_read / bytes_per_frame;
		_current_frame += frames_read;
		return frames_read;
	}

	bool OggVorbisDecoder::seek_frame(uint64_t frame)
	{
		if (frame > _total_frames)
			return false;
		if (::ov_pcm_seek(&_ov_file, static_cast<ogg_int64_t>(frame)) != 0)
			return false;
		_current_frame = frame;
		return true;
	}
}
