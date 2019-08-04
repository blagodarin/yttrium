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

#include <yttrium/audio/api.h>

#include <chrono>
#include <memory>

namespace Yt
{
	class AudioDecoder;
	class AudioFormat;
	class Source;

	/// Read-only audio file access interface.
	class Y_AUDIO_API AudioReader
	{
	public:
		///
		explicit AudioReader(std::unique_ptr<Source>&&);
		~AudioReader() noexcept;

		/// Returns audio format.
		AudioFormat format() const noexcept;

		///
		size_t read_frames(void* buffer, size_t frames);

		/// Moves the audio offset to the specified position in frames.
		bool seek_frame(uint64_t frame);

		///
		bool set_loop(std::chrono::milliseconds from, std::chrono::milliseconds to = std::chrono::milliseconds::max()) noexcept;

		AudioReader(const AudioReader&) = delete;
		AudioReader& operator=(const AudioReader&) = delete;

	private:
		const std::unique_ptr<AudioDecoder> _decoder;
		uint64_t _end_frame;
		uint64_t _loop_frame;
	};
}
