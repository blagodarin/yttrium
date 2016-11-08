/// \file
/// \brief

#ifndef _include_yttrium_audio_player_h_
#define _include_yttrium_audio_player_h_

namespace Yttrium
{
	class Music;
	template <typename> class ResourcePtr;

	/// Audio player.
	class AudioPlayer
	{
	public:
		/// Playback order.
		enum Order
		{
			Loop,    ///< Play the whole list in a loop.
			Random,  ///< Play one random music in an advanced loop.
			Shuffle, ///< Play the shuffled list in a loop.
		};

		virtual ~AudioPlayer() = default;

		/// Loads a music file for playback.
		virtual void load(const ResourcePtr<Music>&) = 0;

		/// Clear the playlist.
		virtual void clear() = 0;

		/// Sets the playback order.
		virtual void set_order(Order) = 0;

		/// Starts or resumes the playback.
		virtual void play() = 0;

		/// Pauses the playback.
		virtual void pause() = 0;

		/// Stops the playback.
		virtual void stop() = 0;

		/// Checks if music is currently playing.
		virtual bool is_playing() const = 0;
	};
}

#endif
