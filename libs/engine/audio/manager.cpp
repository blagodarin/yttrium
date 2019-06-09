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

#include <yttrium/audio/manager.h>

#include <yttrium/audio/reader.h>
#include <yttrium/audio/sound.h>
#include "backend.h"
#include "manager.h"
#include "player.h"

#include <cmath>

namespace Yttrium
{
	AudioManagerPrivate::AudioManagerPrivate()
		: _backend{ AudioBackend::create() }
		, _thread{ [this]{ run(); }}
	{
	}

	AudioManagerPrivate::~AudioManagerPrivate() noexcept
	{
		_done = true;
		_thread.join();
	}

	void AudioManagerPrivate::run()
	{
		const auto buffer = _backend->buffer_view();
		const auto format = _backend->buffer_format();
		if (format.bytes_per_sample() != 2)
			return;
		const auto buffer_frames = buffer.size() / format.block_size();
		constexpr auto frequency = 100.;
		const auto time_scale = frequency / static_cast<double>(format.samples_per_second());
		uint64_t i = 0;
		while (!_done)
		{
			auto output = static_cast<int16_t*>(buffer.data());
			for (auto f = buffer_frames; f > 0; --f)
			{
				const auto amplitude = std::sin(2. * M_PI * time_scale * static_cast<double>(i++));
				const auto amplitude_scale = static_cast<double>(format.samples_per_second() - i % format.samples_per_second()) / static_cast<double>(format.samples_per_second());
				const auto sample = static_cast<int16_t>(std::numeric_limits<int16_t>::max() * amplitude * amplitude_scale);
				for (auto c = format.channels(); c > 0; --c)
					*output++ = sample;
			}
			if (!_backend->write_buffer(_done))
				_done = true;
		}
		// TODO: Wait for the written data to be processed.
	}

	AudioManager::AudioManager()
		: _private{ std::make_unique<AudioManagerPrivate>() }
	{
	}

	AudioManager::~AudioManager() noexcept = default;

	std::unique_ptr<Sound> AudioManager::create_sound(std::unique_ptr<Source>&& source)
	{
		const auto reader = AudioReader::open(std::move(source));
		return reader ? _private->backend().create_sound(*reader) : nullptr;
	}
}
