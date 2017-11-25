#ifndef _src_audio_player_h_
#define _src_audio_player_h_

#include <yttrium/audio/player.h>

#include <condition_variable>
#include <mutex>
#include <thread>

namespace Yttrium
{
	class AudioBackend;

	class AudioPlayerPrivate
	{
	public:
		AudioPlayerPrivate(AudioBackend&, AudioPlayer::State);
		~AudioPlayerPrivate();

		void set_music(const std::shared_ptr<MusicReader>&);
		void set_state(AudioPlayer::State);

	private:
		void run();

	private:
		AudioBackend& _backend;
		std::mutex _mutex;
		std::condition_variable _condition;
		AudioPlayer::State _state = AudioPlayer::State::Stopped;
		std::shared_ptr<MusicReader> _music;
		bool _music_changed = false;
		bool _terminate = false;
		std::thread _thread;
	};
}

#endif
