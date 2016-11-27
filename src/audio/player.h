#ifndef _src_audio_player_h_
#define _src_audio_player_h_

#include <yttrium/audio/player.h>

#include "../base/thread_buffer.h"
#include "playlist.h"
#include "streamer.h"

#include <thread>

namespace Yttrium
{
	class AudioPlayerPrivate
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

		AudioPlayerPrivate(std::unique_ptr<AudioPlayerBackend>&&, Allocator&);
		~AudioPlayerPrivate();

		AudioPlaylist& playlist() { return _playlist; }
		void push_action(Action action) { _action.write(action); }
		State state() const { return _state; }

	private:
		void run();

	private:
		Allocator& _allocator;
		AudioPlaylist _playlist;
		ThreadBuffer<Action> _action;
		State _state = Stopped;
		const std::unique_ptr<AudioPlayerBackend> _backend;
		AudioStreamer _streamer;
		std::thread _thread;
	};
}

#endif
