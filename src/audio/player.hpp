#ifndef __AUDIO_PLAYER_HPP
#define __AUDIO_PLAYER_HPP

#include <Yttrium/audio/player.hpp>
#include <Yttrium/thread.hpp>
#include <Yttrium/thread_buffer.hpp>

#include "backend/player.hpp"
#include "playlist.hpp"
#include "streamer.hpp"

namespace Yttrium
{

class AudioPlayer::Private: Thread
{
public:

	enum State
	{
		Stopped,
		Paused,
		Playing,
	};

	enum Action
	{
		Play,
		Pause,
		Stop,
		Exit,
	};

	typedef ThreadBuffer<Action> ActionBuffer;

public:

	AudioPlaylist _playlist;
	ActionBuffer  _action;
	State         _state;

public:

	Private(Allocator *allocator);

	~Private() noexcept;

protected:

	virtual void run();

private:

	Allocator          *_allocator;
	AudioPlayerBackend *_backend;
	AudioStreamer       _streamer;
};

} // namespace Yttrium

#endif // __AUDIO_PLAYER_HPP
