#ifndef __AUDIO_PLAYER_H
#define __AUDIO_PLAYER_H

#include <yttrium/audio/player.h>

#include "../base/thread_buffer.h"
#include "backend/player.h"
#include "playlist.h"
#include "streamer.h"

#include <thread>

namespace Yttrium
{

class Y_PRIVATE AudioPlayer::Private
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

	AudioPlaylist        _playlist;
	ThreadBuffer<Action> _action;
	State                _state;

public:

	Private(Allocator *allocator);
	~Private();

private:

	void run();

private:

	Allocator          *_allocator;
	AudioPlayerBackend *_backend;
	AudioStreamer       _streamer;
	std::thread         _thread;
};

} // namespace Yttrium

#endif // __AUDIO_PLAYER_H
