#include "player.h"

#include <yttrium/audio/manager.h>
#include <yttrium/audio/music.h>
#include "backend.h"
#include "manager.h"
#include "streamer.h"

namespace Yttrium
{
	AudioPlayerPrivate::AudioPlayerPrivate(std::unique_ptr<AudioPlayerBackend>&& backend, AudioPlayer::State initial_state)
		: _backend(std::move(backend))
		, _state(initial_state)
		, _thread([this]{ run(); })
	{
	}

	AudioPlayerPrivate::~AudioPlayerPrivate()
	{
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_terminate = true;
		}
		_condition.notify_one();
		_thread.join();
	}

	void AudioPlayerPrivate::set_music(const ResourcePtr<const Music>& music)
	{
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_music = music;
			if (!_music || _state != AudioPlayer::State::Playing)
				return;
		}
		_condition.notify_one();
	}

	void AudioPlayerPrivate::set_state(AudioPlayer::State state)
	{
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_state = state;
			if (!_music || _state != AudioPlayer::State::Playing)
				return;
		}
		_condition.notify_one();
	}

	void AudioPlayerPrivate::run()
	{
		AudioStreamer streamer(*_backend); // TODO: Move inside the loop.
		for (;;)
		{
			ResourcePtr<const Music> music;
			{
				std::unique_lock<std::mutex> lock(_mutex);
				_condition.wait(lock, [this]{ return (_state == AudioPlayer::State::Playing && _music) || _terminate; });
				if (_terminate)
					break;
				music = _music;
			}
			if (!streamer.open(music))
			{
				std::lock_guard<std::mutex> lock(_mutex);
				if (_music == music)
					_music = {};
				continue;
			}
			streamer.prefetch();
			_backend->play();
			for (bool restart = false; !restart;)
			{
				{
					std::unique_lock<std::mutex> lock(_mutex);
					if (_condition.wait_for(lock, std::chrono::milliseconds(300), [this, &music]{ return _state != AudioPlayer::State::Playing || _music != music || _terminate; }))
					{
						_condition.wait(lock, [this, &music]{ return _state == AudioPlayer::State::Playing || _music != music || _terminate; });
						if (_music != music || _terminate)
							break;
					}
				}
				for (auto i = _backend->check_buffers(); i > 0; --i)
				{
					const auto fetch_result = streamer.fetch();
					if (fetch_result == AudioStreamer::Ok)
						continue;
					if (!streamer.open(music))
					{
						std::lock_guard<std::mutex> lock(_mutex);
						if (_music == music)
							_music = {};
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
			_backend->stop();
			streamer.close();
		}
	}

	AudioPlayer::AudioPlayer(AudioManager& manager, State initial_state)
		: _private(std::make_unique<AudioPlayerPrivate>(manager._private->create_player_backend(), initial_state))
	{
	}

	AudioPlayer::~AudioPlayer() = default;

	void AudioPlayer::set_music(const ResourcePtr<const Music>& music)
	{
		_private->set_music(music);
	}

	void AudioPlayer::set_state(State state)
	{
		_private->set_state(state);
	}
}
