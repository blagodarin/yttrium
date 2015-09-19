/// \file
/// \brief Audio player.

#ifndef _include_yttrium_audio_player_h_
#define _include_yttrium_audio_player_h_

#include <yttrium/audio/io.h>

namespace Yttrium
{
	/// Audio player.
	class AudioPlayer
	{
	public:

		/// Structure containing audio start, end and loop (re)start times in samples.
		/// \note No loop if \a loop >= \a end.
		struct Settings
		{
			double begin = 0; ///< Position to begin playback at.
			double end = 0;   ///< Position to end playback at.
			double loop = 0;  ///< Position to loop playback at.
		};

		/// Playback order.
		enum Order
		{
			Loop,    ///< Play the whole list in a loop.
			Random,  ///< Play one random music in an advanced loop.
			Shuffle, ///< Play the shuffled list in a loop.
		};

		virtual ~AudioPlayer() = default;

		/// Load an audio file for playback.
		/// \param name Audio file name.
		/// \param settings Playback parameters.
		/// \param type Audio file type.
		virtual void load(const StaticString& name, const Settings& settings = {}, AudioType type = AudioType::Auto) = 0;

		/**
		* \overload
		* \param name Audio file name.
		* \param type Audio file type.
		*/
		void load(const StaticString& name, AudioType type)
		{
			load(name, {}, type);
		}

		/// Clear the playlist.
		virtual void clear() = 0;

		/// Set the playback order.
		/// \param order Playback order.
		virtual void set_order(Order order) = 0;

		/// Start or resume the playback.
		virtual void play() = 0;

		/// Pause the playback.
		virtual void pause() = 0;

		/// Stop the playback.
		virtual void stop() = 0;

		/// Check if music is currently playing.
		/// \return Playback status.
		virtual bool is_playing() const = 0;
	};
}

#endif
