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

#include "manager.h"

#include <yttrium/audio/reader.h>
#include <yttrium/audio/sound.h>
#include <yttrium/memory/buffer.h>
#include "backend.h"
#include "sound.h"

#include <cstring>

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
	AudioManagerImpl::AudioManagerImpl(std::unique_ptr<AudioBackend>&& backend)
		: _backend{ std::move(backend) }
		, _thread{ [this] { run(); } }
	{
	}

	AudioManagerImpl::~AudioManagerImpl() noexcept
	{
		_done = true;
		_condition.notify_one();
		_thread.join();
	}

	std::shared_ptr<Sound> AudioManagerImpl::create_sound(std::unique_ptr<Source>&& source)
	{
		return source ? std::make_shared<SoundImpl>(std::move(source)) : nullptr;
	}

	void AudioManagerImpl::play_music(const std::shared_ptr<AudioReader>& music)
	{
		{
			std::scoped_lock lock{ _mutex };
			if (music == _music)
				return;
			_music = music;
		}
		_condition.notify_one();
	}

	void AudioManagerImpl::play_sound(const std::shared_ptr<Sound>&)
	{
	}

	void AudioManagerImpl::run()
	{
		::set_high_priority();
		const auto buffer_info = _backend->buffer_info();
		Buffer buffer{ buffer_info._size };
		for (;;)
		{
			decltype(_music) music;
			{
				std::unique_lock lock{ _mutex };
				_condition.wait(lock, [this] { return _music || _done; });
				if (_done)
					break;
				if (_music->format() != buffer_info._format)
				{
					_music.reset();
					continue;
				}
				music = _music;
			}
			music->seek(0);
			for (;;)
			{
				const auto bytes_read = music->read(buffer.data(), buffer_info._size);
				if (!bytes_read)
					break;
				if (bytes_read < buffer_info._size)
					std::memset(buffer.begin() + bytes_read, 0, buffer_info._size - bytes_read);
				if (!_backend->write_buffer(buffer.begin(), _done))
					_done = true;
				std::scoped_lock lock{ _mutex };
				if (_music != music || _done)
					break;
			}
		}
		_backend->flush();
	}

	std::shared_ptr<AudioManager> AudioManager::create()
	{
		return std::make_shared<AudioManagerImpl>(AudioBackend::create());
	}
}
