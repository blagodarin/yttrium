//
// Copyright 2019 Sergei Blagodarin
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

#include <yttrium/audio/reader.h>

#include <yttrium/audio/format.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>

namespace Yttrium
{
	class AudioReaderImpl : public AudioReader
	{
		friend AudioReader;

	public:
		uint64_t current_frame() const noexcept override { return _current_frame; }
		AudioFormat format() const noexcept override { return _format; }
		uint64_t total_bytes() const noexcept override { return _total_frames * _format.frame_bytes(); }
		uint64_t total_frames() const noexcept override { return _total_frames; }

	protected:
		AudioFormat _format;
		uint64_t _total_frames = 0;
		uint64_t _current_frame = 0;
		const std::unique_ptr<Source> _source;
		Reader _reader{ *_source };

		explicit AudioReaderImpl(std::unique_ptr<Source>&& source)
			: _source{ std::move(source) } {}
	};
}
