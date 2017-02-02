#ifndef _src_audio_player_h_
#define _src_audio_player_h_

#include <yttrium/audio/player.h>
#include <yttrium/resources/resource_ptr.h>

#include <condition_variable>
#include <mutex>
#include <thread>

namespace Yttrium
{
	class AudioPlayerBackend;

	class AudioPlayerPrivate
	{
	public:
		AudioPlayerPrivate(std::unique_ptr<AudioPlayerBackend>&&, AudioPlayer::State);
		~AudioPlayerPrivate();

		void set_music(const ResourcePtr<const Music>&);
		void set_state(AudioPlayer::State);

	private:
		void run();

	private:
		const std::unique_ptr<AudioPlayerBackend> _backend;
		std::mutex _mutex;
		std::condition_variable _condition;
		AudioPlayer::State _state = AudioPlayer::State::Stopped;
		ResourcePtr<const Music> _music;
		bool _terminate = false;
		std::thread _thread;
	};
}

#endif
