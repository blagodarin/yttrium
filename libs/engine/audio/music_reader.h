//
// Copyright 2018 Sergei Blagodarin
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

#ifndef _src_audio_music_reader_h_
#define _src_audio_music_reader_h_

#include <yttrium/audio/music_reader.h>

#include <memory>

namespace Yttrium
{
	class AudioFormat;
	class AudioReader;

	class MusicReaderImpl final : public MusicReader
	{
	public:
		explicit MusicReaderImpl(std::unique_ptr<AudioReader>&&);

		bool set_properties(int start_ms, int end_ms, int loop_ms) noexcept override;

		size_t buffer_size() const noexcept { return _buffer_samples * _block_size; }
		AudioFormat format() const noexcept;
		size_t read(void*);
		void seek_start();

	private:
		const std::unique_ptr<AudioReader> _reader;
		const size_t _block_size;
		const size_t _buffer_samples;
		uint64_t _start_sample = 0;
		uint64_t _end_sample = 0;
		uint64_t _loop_sample = 0;
	};
}

#endif
