#include "player.h"

#include <yttrium/audio/manager.h>
#include <yttrium/audio/music.h>
#include "backend.h"
#include "manager.h"
#include "streamer.h"

namespace Yttrium
{
	AudioPlayerPrivate::AudioPlayerPrivate(AudioBackend& backend, AudioPlayer::State initial_state)
		: _backend(backend)
		, _state(initial_state)
		, _thread([this]{ run(); })
	{
	}

	AudioPlayerPrivate::~AudioPlayerPrivate()
	{
		{
			std::lock_guard<std::mutex> lock{_mutex};
			_terminate = true;
		}
		_condition.notify_one();
		_thread.join();
	}

	void AudioPlayerPrivate::set_music(const std::shared_ptr<const Music>& music)
	{
		{
			std::lock_guard<std::mutex> lock{_mutex};
			_music = music;
			_music_changed = true;
			if (!_music || _state != AudioPlayer::State::Playing)
				return;
		}
		_condition.notify_one();
	}

	void AudioPlayerPrivate::set_state(AudioPlayer::State state)
	{
		{
			std::lock_guard<std::mutex> lock{_mutex};
			_state = state;
			if (!_music || _state != AudioPlayer::State::Playing)
				return;
		}
		_condition.notify_one();
	}

	void AudioPlayerPrivate::run()
	{
		const auto player_backend = _backend.create_player();
		for (;;)
		{
			std::shared_ptr<const Music> music;
			{
				std::unique_lock<std::mutex> lock{_mutex};
				if (!_music_changed)
					_music.reset();
				_condition.wait(lock, [this]{ return (_state == AudioPlayer::State::Playing && _music) || _terminate; });
				if (_terminate)
					break;
				music = _music;
				_music_changed = false;
			}
			AudioStreamer streamer(*player_backend);
			if (!streamer.open(music))
				continue;
			streamer.start();
			for (bool restart = false; !restart;)
			{
				{
					std::unique_lock<std::mutex> lock{_mutex};
					if (_condition.wait_for(lock, std::chrono::milliseconds{300}, [this]{ return _state != AudioPlayer::State::Playing || _music_changed || _terminate; }))
					{
						_condition.wait(lock, [this]{ return _state == AudioPlayer::State::Playing || _music_changed || _terminate; });
						if (_music_changed || _terminate)
							break;
					}
				}
				for (auto i = player_backend->check_buffers(); i > 0; --i)
				{
					const auto fetch_result = streamer.fetch();
					if (fetch_result == AudioStreamer::Ok)
						continue;
					if (!streamer.open(music))
					{
						restart = true;
						break;
					}
					if (fetch_result == AudioStreamer::NoMoreData)
					{
						if (streamer.fetch() != AudioStreamer::Ok)
							std::abort(); // A music must be at least a buffer long, so any music must fill at least one buffer. // TODO: Better behavior.
					}
				}
			}
		}
	}

	AudioPlayer::AudioPlayer(AudioManager& manager, State initial_state)
		: _private{std::make_unique<AudioPlayerPrivate>(manager._private->backend(), initial_state)}
	{
	}

	AudioPlayer::~AudioPlayer() = default;

	void AudioPlayer::set_music(const std::shared_ptr<const Music>& music)
	{
		_private->set_music(music);
	}

	void AudioPlayer::set_state(State state)
	{
		_private->set_state(state);
	}
}
