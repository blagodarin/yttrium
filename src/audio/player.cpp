#include "player.h"

#include <yttrium/audio/manager.h>
#include "backend.h"
#include "manager.h"

namespace Yttrium
{
	enum
	{
		FetchDelay = 300, ///< 300 ms interval between the successive fetches.
	};

	AudioPlayerPrivate::AudioPlayerPrivate(std::unique_ptr<AudioPlayerBackend>&& backend, Allocator& allocator)
		: _allocator(allocator)
		, _backend(std::move(backend))
		, _streamer(*_backend, _allocator)
		, _thread([this]{ run(); })
	{
	}

	AudioPlayerPrivate::~AudioPlayerPrivate()
	{
		_action.write(Exit);
		_thread.join();
	}

	void AudioPlayerPrivate::set_music(const ResourcePtr<const Music>& music)
	{
		std::lock_guard<std::mutex> lock(_music_mutex);
		_music = music;
	}

	void AudioPlayerPrivate::run()
	{
		const auto next = [this]
		{
			std::lock_guard<std::mutex> lock(_music_mutex);
			return _music;
		};

		for (;;)
		{
			Action action = _action.read();
			if (action == Play)
			{
				if (_streamer.open(next()))
				{
					_streamer.prefetch();
					_backend->play();
					_state = Playing;
					while (action == Play)
					{
						action = _action.read(FetchDelay, Play);
						switch (action)
						{
						case Play:

							for (size_t i = _backend->check_buffers(); i > 0; --i)
							{
								AudioStreamer::FetchResult fetch_result = _streamer.fetch();

								if (fetch_result != AudioStreamer::Ok)
								{
									if (_streamer.open(next()))
									{
										if (fetch_result == AudioStreamer::NoMoreData)
										{
											// NOTE: Since audio shorter than a buffer can't be loaded into a playlist,
											// we can check nothing here.
											_streamer.fetch();
										}
									}
									else
									{
										action = Stop;
										break;
									}
								}
							}
							break;

						case Pause:

							_state = Paused;
							_backend->pause();
							do action = _action.read(); while (action == Pause);
							if (action == Play)
							{
								_backend->play();
								_state = Playing;
								break;
							}
							// Fallthrough.

						case Stop:
						case Exit:

							break;
						}
					}

					if (action == Stop || action == Exit)
					{
						_state = Stopped;
						_backend->stop();
						_streamer.close();
					}
				}
			}

			if (action == Exit)
				break;
		}
	}

	AudioPlayer::AudioPlayer(AudioManager& manager)
		: _private(std::make_unique<AudioPlayerPrivate>(manager._private->create_player_backend(), manager._private->_allocator))
	{
	}

	AudioPlayer::~AudioPlayer() = default;

	void AudioPlayer::pause()
	{
		_private->push_action(AudioPlayerPrivate::Pause);
	}

	void AudioPlayer::play(const ResourcePtr<const Music>& music)
	{
		_private->push_action(AudioPlayerPrivate::Stop);
		_private->set_music(music);
		_private->push_action(AudioPlayerPrivate::Play);
	}

	void AudioPlayer::resume()
	{
		_private->push_action(AudioPlayerPrivate::Play);
	}
}
