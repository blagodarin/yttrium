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

#ifdef _WIN32
// TODO: Implement.
#else
#	include <pthread.h>
#	include <sched.h>
#endif

namespace
{
	void set_high_priority() noexcept
	{
#ifdef _WIN32
		// TODO: Implement.
#else
		const auto thread = pthread_self();
		int policy;
		sched_param param;
		if (pthread_getschedparam(thread, &policy, &param) == 0)
			if (param.sched_priority = sched_get_priority_max(policy); param.sched_priority != -1)
				pthread_setschedparam(thread, policy, &param);
#endif
	}
}

namespace Yttrium
{
	AudioManagerPrivate::AudioManagerPrivate()
		: _backend{ AudioBackend::create() }
		, _thread{ [this] { run(); } }
	{
	}

	AudioManagerPrivate::~AudioManagerPrivate() noexcept
	{
		_done = true;
		_thread.join();
	}

	void AudioManagerPrivate::run()
	{
		::set_high_priority();
		const auto buffer_info = _backend->buffer_info();
		if (buffer_info._format.bytes_per_sample() != 2)
			return;
		Buffer buffer{ buffer_info._size };
		const auto buffer_frames = buffer_info._size / buffer_info._format.block_size();
		constexpr auto frequency = 100.;
		const auto rate = buffer_info._format.samples_per_second();
		const auto time_scale = frequency / static_cast<double>(rate);
		uint64_t i = 0;
		while (!_done)
		{
			auto output = static_cast<int16_t*>(buffer.data());
			for (auto f = buffer_frames; f > 0; --f)
			{
				const auto amplitude = std::sin(2. * M_PI * time_scale * static_cast<double>(i++));
				const auto amplitude_scale = 1. - static_cast<double>(i % rate) / static_cast<double>(rate);
				const auto sample = static_cast<int16_t>(std::numeric_limits<int16_t>::max() * amplitude * amplitude_scale);
				for (auto c = buffer_info._format.channels(); c > 0; --c)
					*output++ = sample;
			}
			if (!_backend->write_buffer(buffer.begin(), _done))
				_done = true;
		}
		_backend->flush();
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
