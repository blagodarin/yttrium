//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include <cstddef>

namespace Yttrium
{
	enum class AudioSample
	{
		i16,
		f32,
	};

	class AudioFormat
	{
	public:
		constexpr AudioFormat() noexcept = default;
		constexpr AudioFormat(AudioSample sample_type, size_t channels, size_t frames_per_second) noexcept
			: _sample_type{ sample_type }, _channels{ channels }, _frames_per_second{ frames_per_second } {}

		constexpr size_t bytes_per_frame() const noexcept { return bytes_per_sample() * _channels; }
		constexpr size_t bytes_per_sample() const noexcept { return bytes_per_sample(_sample_type); }
		constexpr size_t bytes_per_second() const noexcept { return bytes_per_frame() * _frames_per_second; }
		constexpr size_t channels() const noexcept { return _channels; }
		constexpr size_t frames_per_second() const noexcept { return _frames_per_second; }
		constexpr AudioSample sample_type() const noexcept { return _sample_type; }

		static constexpr size_t bytes_per_sample(AudioSample type) noexcept
		{
			switch (type)
			{
			case AudioSample::i16: return 2;
			case AudioSample::f32: return 4;
			}
			return 0;
		}

	private:
		AudioSample _sample_type = AudioSample::i16;
		size_t _channels = 0;
		size_t _frames_per_second = 0;
	};

	constexpr bool operator==(const AudioFormat& a, const AudioFormat& b) noexcept { return a.sample_type() == b.sample_type() && a.channels() == b.channels() && a.frames_per_second() == b.frames_per_second(); }
	constexpr bool operator!=(const AudioFormat& a, const AudioFormat& b) noexcept { return !(a == b); }
}
