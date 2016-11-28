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
		/// Playback order.
		enum Order
		{
			Loop,    ///< Play the whole list in a loop.
			Random,  ///< Play one random music in an advanced loop.
			Shuffle, ///< Play the shuffled list in a loop.
		};

		///
		AudioPlayer(AudioManager&);

		///
		~AudioPlayer();

		/// Loads a music file for playback.
		void load(const ResourcePtr<const Music>&);

		/// Clears the playlist.
		void clear();

		/// Sets the playback order.
		void set_order(Order);

		/// Starts or resumes the playback.
		void play();

		/// Pauses the playback.
		void pause();

		/// Stops the playback.
		void stop();

		/// Checks if music is currently playing.
		bool is_playing() const;

	private:
		const std::unique_ptr<class AudioPlayerPrivate> _private;
	};
}

#endif
