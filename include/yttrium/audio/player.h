/// \file
/// \brief

#ifndef _include_yttrium_audio_player_h_
#define _include_yttrium_audio_player_h_

#include <yttrium/api.h>

#include <memory>

namespace Yttrium
{
	class AudioManager;
	class Music;
	template <typename> class ResourcePtr;

	/// Audio player.
	class Y_API AudioPlayer
	{
	public:
		///
		AudioPlayer(AudioManager&);

		///
		~AudioPlayer();

		///
		void pause();

		///
		void play(const ResourcePtr<const Music>&);

		///
		void resume();

	private:
		const std::unique_ptr<class AudioPlayerPrivate> _private;
	};
}

#endif
