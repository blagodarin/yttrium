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

#include "mixer.h"
#include "sound.h"

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
			_commands.emplace_back(PlayMusic{ music });
		}
		_condition.notify_one();
	}

	void AudioManagerImpl::play_sound(const std::shared_ptr<Sound>& sound)
	{
		{
			std::scoped_lock lock{ _mutex };
			_commands.emplace_back(PlaySound{ sound });
		}
		_condition.notify_one();
	}

	void AudioManagerImpl::run()
	{
		struct Context
		{
			AudioMixer _mixer;
			explicit Context(const AudioBackend::BufferInfo& buffer_info) noexcept
				: _mixer{ buffer_info } {}
			void operator()(const PlayMusic& command) { _mixer.play_music(command._music); }
			void operator()(const PlaySound& command) { _mixer.play_sound(command._sound); }
		};

		::set_high_priority();
		for (Context context{ _backend->buffer_info() };;)
		{
			Command command;
			{
				std::unique_lock lock{ _mutex };
				_condition.wait(lock, [this] { return !_commands.empty() || _done; });
				if (_done)
					break;
				command = std::move(_commands.front());
				_commands.pop_front();
			}
			std::visit(context, command);
			while (const auto buffer = context._mixer.mix_buffer())
			{
				if (!_backend->write_buffer(buffer, _done))
					_done = true;
				if (_done)
					break;
				{
					std::scoped_lock lock{ _mutex };
					if (_commands.empty())
						continue;
					command = std::move(_commands.front());
					_commands.pop_front();
				}
				std::visit(context, command);
			}
		}
		_backend->flush();
	}

	std::shared_ptr<AudioManager> AudioManager::create()
	{
		return std::make_shared<AudioManagerImpl>(AudioBackend::create());
	}
}
