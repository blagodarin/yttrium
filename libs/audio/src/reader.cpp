// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/audio/reader.h>

#include <yttrium/base/numeric.h>
#include "decoder.h"

#include <algorithm>
#include <cassert>

namespace Yt
{
	AudioReader::AudioReader(std::unique_ptr<Source>&& source, bool looping)
		: _decoder{ AudioDecoder::create(std::move(source), looping) }
		, _end_frame{ _decoder->total_frames() }
		, _loop_frame{ _end_frame }
	{
	}

	AudioReader::~AudioReader() noexcept = default;

	AudioFormat AudioReader::format() const noexcept
	{
		return _decoder->format();
	}

	size_t AudioReader::read_frames(void* buffer, size_t frames)
	{
		const auto frame_size = _decoder->format().bytes_per_frame();
		for (size_t result = 0;;)
		{
			const auto requested = frames - result;
			const auto available = _end_frame - _decoder->current_frame();
			const auto decoded = _decoder->read_frames(static_cast<std::byte*>(buffer) + result * frame_size, static_cast<size_t>(std::min<uint64_t>(requested, available)));
			result += decoded;
			if (decoded == requested || decoded != available || _loop_frame >= _end_frame || !_decoder->seek_frame(_loop_frame))
				return result;
		}
	}

	void AudioReader::restart()
	{
		_decoder->seek_frame(0);
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
}
