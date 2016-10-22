/// \file
/// \brief Sounds.

#ifndef _include_yttrium_audio_sound_h_
#define _include_yttrium_audio_sound_h_

#include <yttrium/resource/resource.h>

namespace Yttrium
{
	/// %Sound.
	class Sound : public Resource
	{
	public:
		/// Plays the sound asynchronously.
		virtual void play() const = 0; // TODO: Use some sound player to play sounds.
	};
}

#endif
