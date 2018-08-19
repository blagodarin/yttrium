#ifndef _src_audio_player_h_
#define _src_audio_player_h_

#include <yttrium/audio/player.h>

#include <condition_variable>
#include <mutex>
#include <thread>

namespace Yttrium
{
	class AudioBackend;
	class MusicReaderImpl;

	class AudioPlayerPrivate
	{
	public:
		explicit AudioPlayerPrivate(AudioBackend&);
		~AudioPlayerPrivate() noexcept;

		void set_music(const std::shared_ptr<MusicReaderImpl>&);
		void set_paused(bool);

	private:
		void run();

	private:
		AudioBackend& _backend;
		std::mutex _mutex;
		std::condition_variable _condition;
		bool _paused = false;
		std::shared_ptr<MusicReaderImpl> _music;
		bool _music_changed = false;
		bool _terminate = false;
		std::thread _thread;
	};
}

#endif
