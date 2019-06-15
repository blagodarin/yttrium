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

#include <yttrium/audio/reader.h>

namespace Yttrium
{
	class AudioFormat;
	class AudioReader;

	class MusicReaderImpl final : public MusicReader
	{
	public:
		explicit MusicReaderImpl(std::unique_ptr<Source>&&);

		bool set_properties(int end_ms, int loop_ms) noexcept override;

		size_t buffer_size() const noexcept { return _buffer_frames * _frame_bytes; }
		AudioFormat format() const noexcept;
		size_t read(void*);
		void seek_start();

	private:
		AudioReader _reader;
		const size_t _frame_bytes;
		const size_t _buffer_frames;
		uint64_t _end_frame = 0;
		uint64_t _loop_frame = 0;
	};
}
