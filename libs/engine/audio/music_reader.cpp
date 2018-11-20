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

#include "music_reader.h"

#include <yttrium/audio/format.h>
#include <yttrium/audio/reader.h>
#include <yttrium/utils/numeric.h>
#include "backend.h"

#include <algorithm>

namespace Yttrium
{
	MusicReaderImpl::MusicReaderImpl(std::unique_ptr<AudioReader>&& reader)
		: _reader{ std::move(reader) }
		, _block_size{ _reader->format().block_size() }
		, _buffer_samples{ _reader->format().samples_per_second() }
		, _end_sample{ _reader->total_samples() }
		, _loop_sample{ _end_sample }
	{
	}

	bool MusicReaderImpl::set_properties(int start_ms, int end_ms, int loop_ms) noexcept
	{
		if (start_ms < 0 || !(end_ms == 0 || end_ms >= start_ms) || !(loop_ms >= 0 && loop_ms <= end_ms))
			return false;

		const auto total_samples = _reader->total_samples();
		const auto samples_per_second = _reader->format().samples_per_second();

		_start_sample = std::min(to_unsigned(start_ms) * samples_per_second / 1000, total_samples);
		_end_sample = end_ms > 0 ? std::min(to_unsigned(end_ms) * samples_per_second / 1000, total_samples) : total_samples;
		_loop_sample = std::min(to_unsigned(loop_ms) * samples_per_second / 1000, _end_sample);
		return true;
	}

	AudioFormat MusicReaderImpl::format() const noexcept
	{
		return _reader->format();
	}

	size_t MusicReaderImpl::read(void* buffer)
	{
		for (size_t result = 0;;)
		{
			const auto capacity = buffer_size() - result;
			const auto data_size = (_end_sample - _reader->current_sample()) * _block_size;
			const auto bytes_read = _reader->read(static_cast<std::byte*>(buffer) + result, std::min(capacity, data_size));
			result += bytes_read;
			if (bytes_read == capacity || bytes_read != data_size || _loop_sample >= _end_sample || !_reader->seek(_loop_sample))
				return result;
		}
	}

	void MusicReaderImpl::seek_start()
	{
		_reader->seek(_start_sample);
	}

	std::unique_ptr<MusicReader> MusicReader::open(std::unique_ptr<Source>&& source)
	{
		return source ? std::make_unique<MusicReaderImpl>(AudioReader::open(std::move(source))) : nullptr;
	}
}
