/// \file
/// \brief

#ifndef _include_yttrium_audio_sound_h_
#define _include_yttrium_audio_sound_h_

#include <yttrium/resources/resource.h>

namespace Yttrium
{
	/// Sound.
	class Sound : public Resource
	{
	public:
		/// Plays the sound asynchronously.
		virtual void play() const = 0;
	};
}

#endif
