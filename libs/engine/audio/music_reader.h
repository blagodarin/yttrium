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

#include <yttrium/audio/music_reader.h>

#include <yttrium/audio/format.h>
#include <yttrium/audio/reader.h>

namespace Yttrium
{
	class AudioFormat;
	class AudioReader;

	class MusicReaderImpl final : public MusicReader
	{
	public:
		explicit MusicReaderImpl(std::unique_ptr<Source>&& source)
			: _reader{ std::move(source) } {}

		bool set_properties(int end_ms, int loop_ms) noexcept override { return _reader.set_loop(std::chrono::milliseconds{ loop_ms }, std::chrono::milliseconds{ end_ms }); }

		size_t buffer_size() const noexcept { return _reader.format().frames_per_second() * _reader.format().frame_bytes(); }
		AudioFormat format() const noexcept { return _reader.format(); }
		size_t read(void* buffer) { return _reader.read(buffer, buffer_size()); }
		void seek_start() { _reader.seek(0); }

	private:
		AudioReader _reader;
	};
}
