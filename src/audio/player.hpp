/// \file
/// \brief Generic backend-independent playlist implementation.

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

class AudioPlayer::Private
{
public:

	Private(Allocator *allocator);

	~Private();

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

	ActionBuffer _action;
	State        _state;

	Allocator          *_allocator;
	AudioPlayerBackend *_backend;
	AudioPlaylist       _playlist;
	AudioStreamer       _streamer;
	Thread              _thread;

private:

	void thread_function();
};

} // namespace Yttrium

#endif // __AUDIO_PLAYER_HPP
