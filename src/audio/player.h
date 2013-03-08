#ifndef __AUDIO_PLAYER_H
#define __AUDIO_PLAYER_H

#include <Yttrium/audio/player.h>
#include <Yttrium/thread.h>
#include <Yttrium/thread_buffer.h>

#include "backend/player.h"
#include "playlist.h"
#include "streamer.h"

namespace Yttrium
{

class Y_PRIVATE AudioPlayer::Private: public Thread
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

	~Private() noexcept override;

protected:

	void run() override;

private:

	Allocator          *_allocator;
	AudioPlayerBackend *_backend;
	AudioStreamer       _streamer;
};

} // namespace Yttrium

#endif // __AUDIO_PLAYER_H
