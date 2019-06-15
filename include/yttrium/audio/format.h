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

#ifndef _include_yttrium_audio_format_h_
#define _include_yttrium_audio_format_h_

#include <cstddef>

namespace Yttrium
{
	class AudioFormat
	{
	public:
		constexpr AudioFormat() noexcept = default;
		constexpr AudioFormat(size_t bytes_per_sample, size_t channels, size_t frames_per_second) noexcept
			: _bytes_per_sample{ bytes_per_sample }, _channels{ channels }, _frames_per_second{ frames_per_second } {}

		constexpr size_t frame_bytes() const noexcept { return _bytes_per_sample * _channels; }
		constexpr size_t bytes_per_sample() const noexcept { return _bytes_per_sample; }
		constexpr size_t channels() const noexcept { return _channels; }
		constexpr size_t bytes_per_second() const noexcept { return _bytes_per_sample * _channels * _frames_per_second; }
		constexpr size_t frames_per_second() const noexcept { return _frames_per_second; }

	private:
		size_t _bytes_per_sample = 0;
		size_t _channels = 0;
		size_t _frames_per_second = 0;
	};

	constexpr bool operator==(const AudioFormat& a, const AudioFormat& b) noexcept { return a.bytes_per_sample() == b.bytes_per_sample() && a.channels() == b.channels() && a.frames_per_second() == b.frames_per_second(); }
	constexpr bool operator!=(const AudioFormat& a, const AudioFormat& b) noexcept { return !(a == b); }
}

#endif
