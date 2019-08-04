//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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
