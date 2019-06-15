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

#include "music_reader.h"

#include <yttrium/audio/format.h>
#include <yttrium/audio/reader.h>
#include <yttrium/utils/numeric.h>
#include "backend.h"

#include <algorithm>

namespace Yttrium
{
	MusicReaderImpl::MusicReaderImpl(std::unique_ptr<Source>&& source)
		: _reader{ std::move(source) }
		, _frame_bytes{ _reader.format().frame_bytes() }
		, _buffer_frames{ _reader.format().frames_per_second() }
		, _end_frame{ _reader.total_frames() }
		, _loop_frame{ _end_frame }
	{
	}

	bool MusicReaderImpl::set_properties(int end_ms, int loop_ms) noexcept
	{
		if (end_ms <= 0 || !(loop_ms >= 0 && loop_ms <= end_ms))
			return false;

		const auto total_frames = _reader.total_frames();
		const auto frames_per_second = uint64_t{ _reader.format().frames_per_second() };

		_end_frame = end_ms > 0 ? std::min(to_unsigned(end_ms) * frames_per_second / 1000, total_frames) : total_frames;
		_loop_frame = std::min(to_unsigned(loop_ms) * frames_per_second / 1000, _end_frame);
		return true;
	}

	AudioFormat MusicReaderImpl::format() const noexcept
	{
		return _reader.format();
	}

	size_t MusicReaderImpl::read(void* buffer)
	{
		for (size_t result = 0;;)
		{
			const auto capacity = buffer_size() - result;
			const auto bytes_left = (_end_frame - _reader.current_frame()) * _frame_bytes;
			const auto bytes_read = _reader.read(static_cast<std::byte*>(buffer) + result, static_cast<size_t>(std::min<uint64_t>(capacity, bytes_left)));
			result += bytes_read;
			if (bytes_read == capacity || bytes_read != bytes_left || _loop_frame >= _end_frame || !_reader.seek(_loop_frame))
				return result;
		}
	}

	void MusicReaderImpl::seek_start()
	{
		_reader.seek(0);
	}

	std::unique_ptr<MusicReader> MusicReader::open(std::unique_ptr<Source>&& source)
	{
		return source ? std::make_unique<MusicReaderImpl>(std::move(source)) : nullptr;
	}
}
