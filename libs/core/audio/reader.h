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
#include "decoder.h"

namespace Yttrium
{
	class AudioReaderImpl final : public AudioReader
	{
	public:
		explicit AudioReaderImpl(std::unique_ptr<AudioDecoder>&& decoder) : _decoder{ std::move(decoder) } {}

		uint64_t current_frame() const noexcept override { return _decoder->current_frame(); }
		AudioFormat format() const noexcept override { return _decoder->format(); }
		size_t read(void* data, size_t bytes) override { return _decoder->read(data, bytes); }
		bool seek(uint64_t frame_offset) override { return _decoder->seek(frame_offset); }
		uint64_t total_bytes() const noexcept override { return _decoder->total_bytes(); }
		uint64_t total_frames() const noexcept override { return _decoder->total_frames(); }

	private:
		const std::unique_ptr<AudioDecoder> _decoder;
	};
}
