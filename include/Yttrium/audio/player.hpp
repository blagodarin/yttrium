/// \file
/// \brief Audio player.

#ifndef __Y_AUDIO_PLAYER_HPP
#define __Y_AUDIO_PLAYER_HPP

#include <Yttrium/audio/io.hpp>

namespace Yttrium
{

class AudioManager;

/// Audio player.
/// \note All AudioPlayer instances refer to the same entity within AudioManager,
/// hence its actual lifetime is the same as the corresponding %AudioManager one.

class Y_API AudioPlayer
{
	friend class AudioManager;

public:

	/// Structure containing audio start, end and loop (re)start times in samples.
	/// \note No loop if \a loop >= \a end.

	struct Settings
	{
		double begin; ///< Position to begin playback at.
		double end;   ///< Position to end playback at.
		double loop;  ///< Position to loop playback at.

		Settings()
			: begin(0)
			, end(0)
			, loop(0)
		{
		}
	};

	/// Playback order.

	enum Order
	{
		Loop,    ///< Play the whole list in a loop.
		Random,  ///< Play one random music in an advanced loop.
		Shuffle, ///< Play the shuffled list in a loop.
	};

public:

	/// Load an audio file for playback.
	/// \param name Audio file name.
	/// \param settings Playback parameters.
	/// \param format Audio file format.

	void load(const StaticString &name, const Settings &settings = Settings(), AudioType type = AudioType::Auto) noexcept;

	/**
	* \overload
	*/

	void load(const StaticString &name, AudioType type) noexcept
	{
		load(name, Settings(), type);
	}

	/// Clear the playlist.

	void clear() noexcept;

	/// Set the playback order.
	/// \param order Playback order.

	void set_order(Order order) noexcept;

	/// Start or resume the playback.

	void play() noexcept;

	/// Pause the playback.

	void pause() noexcept;

	/// Stop the playback.

	void stop() noexcept;

	/// Check if music is currently playing.
	/// \return Playback status.

	bool is_playing() const noexcept;

public:

	class Private;

private:

	AudioPlayer(Private *private_)
		: _private(private_)
	{
	}

private:

	Private *_private;
};

} // namespace Yttrium

#endif // __Y_AUDIO_PLAYER_HPP
