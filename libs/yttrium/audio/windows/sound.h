#ifndef _libs_yttrium_audio_windows_sound_h_
#define _libs_yttrium_audio_windows_sound_h_

#include <yttrium/audio/sound.h>

namespace Yttrium
{
	class WindowsSound final : public Sound
	{
	public:
		explicit WindowsSound(class AudioReader&);

		void play() const override;
	};
}

#endif
