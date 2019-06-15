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

#include <yttrium/audio/player.h>

#include <condition_variable>
#include <mutex>
#include <thread>

namespace Yttrium
{
	class AudioBackend;

	class AudioPlayerPrivate
	{
	public:
		explicit AudioPlayerPrivate(AudioBackend&);
		~AudioPlayerPrivate() noexcept;

		void set_music(const std::shared_ptr<AudioReader>&);
		void set_paused(bool);

	private:
		void run();

	private:
		AudioBackend& _backend;
		std::mutex _mutex;
		std::condition_variable _condition;
		bool _paused = false;
		std::shared_ptr<AudioReader> _music;
		bool _music_changed = false;
		bool _terminate = false;
		std::thread _thread;
	};
}
