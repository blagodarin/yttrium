#include "player.h"

#include "backend.h"
#include "playlist.h"

namespace Yttrium
{
	enum
	{
		FetchDelay = 300, ///< 300 ms interval between the successive fetches.
	};

	AudioPlayerImpl::AudioPlayerImpl(std::unique_ptr<AudioPlayerBackend>&& backend, Allocator& allocator)
		: _allocator(allocator)
		, _playlist(_allocator)
		, _backend(std::move(backend))
		, _streamer(*_backend, _allocator)
		, _thread([this]{ run(); })
	{
	}

	AudioPlayerImpl::~AudioPlayerImpl()
	{
		_action.write(Exit);
		_thread.join();
	}

	void AudioPlayerImpl::load(Reader&& reader, const Settings& settings)
	{
		_playlist.load(std::move(reader), settings);
	}

	void AudioPlayerImpl::clear()
	{
		_playlist.clear();
	}

	void AudioPlayerImpl::set_order(Order order)
	{
		_playlist.set_order(order);
	}

	void AudioPlayerImpl::play()
	{
		_action.write(Play);
	}

	void AudioPlayerImpl::pause()
	{
		_action.write(Pause);
	}

	void AudioPlayerImpl::stop()
	{
		_action.write(Stop);
	}

	bool AudioPlayerImpl::is_playing() const
	{
		return _state == Playing;
	}

	void AudioPlayerImpl::run()
	{
		for (;;)
		{
			Action action = _action.read();
			if (action == Play)
			{
				auto item = _playlist.next();
				if (item.first && _streamer.open(std::move(item.first), item.second, _playlist.order()))
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
									auto item = _playlist.next();
									if (item.first && _streamer.open(std::move(item.first), item.second, _playlist.order()))
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
}
