#ifndef _src_audio_player_h_
#define _src_audio_player_h_

#include <yttrium/audio/player.h>

#include "../base/thread_buffer.h"
#include "playlist.h"
#include "streamer.h"

#include <thread>

namespace Yttrium
{
	class AudioPlayerImpl : public AudioPlayer
	{
	public:

		AudioPlayerImpl(Pointer<AudioPlayerBackend> backend, Allocator* allocator);
		~AudioPlayerImpl() override;

		void load(const StaticString& name, const Settings& settings, AudioType type) override;
		void clear() override;
		void set_order(Order order) override;
		void play() override;
		void pause() override;
		void stop() override;
		bool is_playing() const override;

	private:

		void run();

	private:

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

		Allocator* _allocator;
		AudioPlaylist _playlist;
		ThreadBuffer<Action> _action;
		State _state = Stopped;
		const Pointer<AudioPlayerBackend> _backend;
		AudioStreamer _streamer;
		std::thread _thread;
	};
}

#endif
