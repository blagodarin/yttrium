#ifndef _include_yttrium_audio_player_h_
#define _include_yttrium_audio_player_h_

#include <yttrium/api.h>

#include <memory>

namespace Yttrium
{
	class AudioManager;
	class MusicReader;

	/// Audio player.
	class Y_API AudioPlayer
	{
	public:
		///
		explicit AudioPlayer(AudioManager&);

		///
		~AudioPlayer() noexcept;

		///
		void set_music(const std::shared_ptr<MusicReader>&);

		///
		void set_paused(bool);

	private:
		const std::unique_ptr<class AudioPlayerPrivate> _private;
	};
}

#endif
