#include "player.h"

#include "playlist.h"

namespace Yttrium
{

enum
{
	FetchDelay = 300, ///< 300 ms interval between the successive fetches.
};

AudioPlayer::Private::Private(Allocator *allocator)
	: _playlist(allocator)
	, _state(Stopped)
	, _allocator(allocator)
	, _backend(AudioPlayerBackend::create(_allocator))
	, _streamer(_backend, _allocator)
{
	start();
}

AudioPlayer::Private::~Private()
{
	_action.write(Exit);
	wait();
	Y_DELETE(_allocator, _backend);
}

void AudioPlayer::Private::run()
{
	for (; ; )
	{
		Action action = _action.read();

		if (action == Play)
		{
			AudioPlaylist::Item item;

			if (_playlist.next(&item)
				&& _streamer.open(item.name, item.settings, item.type, _playlist.order()))
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

						for (size_t i = _backend->check_buffers(); i; --i)
						{
							AudioStreamer::FetchResult fetch_result = _streamer.fetch();

							if (fetch_result != AudioStreamer::Ok)
							{
								if (_playlist.next(&item)
									&& _streamer.open(item.name, item.settings, item.type, _playlist.order()))
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
		{
			break;
		}
	}
}

void AudioPlayer::load(const StaticString &name, const Settings &settings, AudioType type)
{
	_private->_playlist.load(name, settings, type);
}

void AudioPlayer::clear()
{
	_private->_playlist.clear();
}

void AudioPlayer::set_order(Order order)
{
	_private->_playlist.set_order(order);
}

void AudioPlayer::play()
{
	_private->_action.write(Private::Play);
}

void AudioPlayer::pause()
{
	_private->_action.write(Private::Pause);
}

void AudioPlayer::stop()
{
	_private->_action.write(Private::Stop);
}

bool AudioPlayer::is_playing() const
{
	return (_private->_state == Private::Playing);
}

} // namespace Yttrium
