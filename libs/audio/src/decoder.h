// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/audio/format.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>

namespace Yt
{
	class AudioDecoder
	{
	public:
		virtual ~AudioDecoder() = default;

		uint64_t current_frame() const noexcept { return _current_frame; }
		AudioFormat format() const noexcept { return _format; }
		uint64_t total_frames() const noexcept { return _total_frames; }

		virtual size_t read_frames(void* buffer, size_t frames) = 0;
		virtual bool seek_frame(uint64_t frame) = 0;

		static std::unique_ptr<Yt::AudioDecoder> create(std::unique_ptr<Source>&&, bool looping);

	protected:
		AudioFormat _format;
		uint64_t _total_frames = 0;
		uint64_t _current_frame = 0;
		const std::unique_ptr<Source> _source;
		Reader _reader{ *_source };

		explicit AudioDecoder(std::unique_ptr<Source>&& source)
			: _source{ std::move(source) } {}
	};
}
