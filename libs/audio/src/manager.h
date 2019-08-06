//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#include <yttrium/audio/manager.h>

#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include <variant>

namespace Yt
{
	class AudioBackend;

	class AudioManagerImpl final : public AudioManager
	{
	public:
		explicit AudioManagerImpl(std::unique_ptr<AudioBackend>&&);
		~AudioManagerImpl() noexcept override;

		std::shared_ptr<Sound> create_sound(std::unique_ptr<Source>&&) override;
		void play_music(const std::shared_ptr<AudioReader>&) override;
		void play_sound(const std::shared_ptr<Sound>&) override;

	private:
		void run();

	private:
		struct PlayMusic
		{
			std::shared_ptr<AudioReader> _music;
		};

		struct PlaySound
		{
			std::shared_ptr<Sound> _sound;
		};

		using Command = std::variant<PlayMusic, PlaySound>;

		const std::unique_ptr<AudioBackend> _backend;
		std::mutex _mutex;
		std::condition_variable _condition;
		std::deque<Command> _commands;
		std::atomic<bool> _done{ false };
		std::thread _thread;
	};
}
