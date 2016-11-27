#include "player.h"

#include <yttrium/audio/manager.h>
#include "backend.h"
#include "manager.h"
#include "playlist.h"

namespace Yttrium
{
	enum
	{
		FetchDelay = 300, ///< 300 ms interval between the successive fetches.
	};

	AudioPlayerPrivate::AudioPlayerPrivate(std::unique_ptr<AudioPlayerBackend>&& backend, Allocator& allocator)
		: _allocator(allocator)
		, _playlist(_allocator)
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

	void AudioPlayerPrivate::run()
	{
		for (;;)
		{
			Action action = _action.read();
			if (action == Play)
			{
				if (_streamer.open(_playlist.next(), _playlist.order()))
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
									if (_streamer.open(_playlist.next(), _playlist.order()))
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

	void AudioPlayer::load(const ResourcePtr<const Music>& music)
	{
		_private->playlist().load(music);
	}

	void AudioPlayer::clear()
	{
		_private->playlist().clear();
	}

	void AudioPlayer::set_order(Order order)
	{
		_private->playlist().set_order(order);
	}

	void AudioPlayer::play()
	{
		_private->push_action(AudioPlayerPrivate::Play);
	}

	void AudioPlayer::pause()
	{
		_private->push_action(AudioPlayerPrivate::Pause);
	}

	void AudioPlayer::stop()
	{
		_private->push_action(AudioPlayerPrivate::Stop);
	}

	bool AudioPlayer::is_playing() const
	{
		return _private->state() == AudioPlayerPrivate::Playing;
	}
}
