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

#include "player.h"

#include <yttrium/audio/manager.h>
#include "backend.h"
#include "manager.h"
#include "music_reader.h"
#include "streamer.h"

namespace Yttrium
{
	AudioPlayerPrivate::AudioPlayerPrivate(AudioBackend& backend)
		: _backend{ backend }
		, _thread{ [this] { run(); } }
	{
	}

	AudioPlayerPrivate::~AudioPlayerPrivate() noexcept
	{
		{
			std::lock_guard<std::mutex> lock{ _mutex };
			_terminate = true;
		}
		_condition.notify_one();
		_thread.join();
	}

	void AudioPlayerPrivate::set_music(const std::shared_ptr<MusicReaderImpl>& music)
	{
		{
			std::lock_guard<std::mutex> lock{ _mutex };
			_music = music;
			_music_changed = true;
			if (_paused || !_music)
				return;
		}
		_condition.notify_one();
	}

	void AudioPlayerPrivate::set_paused(bool paused)
	{
		{
			std::lock_guard<std::mutex> lock{ _mutex };
			_paused = paused;
			if (_paused || !_music)
				return;
		}
		_condition.notify_one();
	}

	void AudioPlayerPrivate::run()
	{
		const auto player_backend = _backend.create_player();
		for (;;)
		{
			decltype(_music) music;
			{
				std::unique_lock<std::mutex> lock{ _mutex };
				if (!std::exchange(_music_changed, false))
					_music.reset();
				_condition.wait(lock, [this] { return (!_paused && _music) || _terminate; });
				if (_terminate)
					break;
				music = _music;
			}
			AudioStreamer streamer{ *player_backend, *music };
			if (!streamer.play_first())
				continue;
			for (bool restart = false; !restart;)
			{
				{
					std::unique_lock<std::mutex> lock{ _mutex };
					if (_condition.wait_for(lock, std::chrono::milliseconds{ 500 }, [this] { return _paused || _music_changed || _terminate; }))
					{
						_condition.wait(lock, [this] { return !_paused || _music_changed || _terminate; });
						if (_music_changed || _terminate)
							break;
					}
				}
				if (!streamer.play_next())
					break;
			}
		}
	}

	AudioPlayer::AudioPlayer(AudioManager& manager)
		: _private{ std::make_unique<AudioPlayerPrivate>(manager._private->backend()) }
	{
	}

	AudioPlayer::~AudioPlayer() noexcept = default;

	void AudioPlayer::set_music(const std::shared_ptr<MusicReader>& music)
	{
		_private->set_music(std::static_pointer_cast<MusicReaderImpl>(music));
	}

	void AudioPlayer::set_paused(bool paused)
	{
		_private->set_paused(paused);
	}
}
