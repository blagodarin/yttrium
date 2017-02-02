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
		enum class State
		{
			Stopped, ///<
			Playing, ///<
		};

		///
		AudioPlayer(AudioManager&, State = State::Stopped);

		///
		~AudioPlayer();

		///
		void set_music(const ResourcePtr<const Music>&);

		///
		void set_state(State);

	private:
		const std::unique_ptr<class AudioPlayerPrivate> _private;
	};
}

#endif
