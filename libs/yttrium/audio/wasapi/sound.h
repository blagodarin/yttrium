#ifndef _libs_yttrium_audio_wasapi_sound_h_
#define _libs_yttrium_audio_wasapi_sound_h_

#include <yttrium/audio/sound.h>

namespace Yttrium
{
	class WasapiSound final : public Sound
	{
	public:
		explicit WasapiSound(class AudioReader&);

		void play() const override;
	};
}

#endif
