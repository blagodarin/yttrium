/// \file
/// \brief Sounds.

#ifndef _include_yttrium_audio_sound_h_
#define _include_yttrium_audio_sound_h_

#include <yttrium/global.h>

namespace Yttrium
{
	/// %Sound.
	class Y_API Sound
	{
	public:
		virtual ~Sound() = default;

		/// Plays the sound asynchronously.
		virtual void play() const = 0;
	};
}

#endif
