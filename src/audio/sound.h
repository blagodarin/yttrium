#ifndef _src_audio_sound_h_
#define _src_audio_sound_h_

#include <yttrium/audio/sound.h>

namespace Yttrium
{
	class AudioReader;

	class SoundImpl : public Sound
	{
	public:
		virtual bool load(AudioReader&) = 0;
	};
}

#endif
