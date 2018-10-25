#ifndef _libs_yttrium_audio_windows_backend_h_
#define _libs_yttrium_audio_windows_backend_h_

#include "../backend.h"

namespace Yttrium
{
	class WindowsAudioBackend final : public AudioBackend
	{
	public:
		WindowsAudioBackend();
		~WindowsAudioBackend() override;

		std::unique_ptr<AudioPlayerBackend> create_player() override;
		std::unique_ptr<Sound> create_sound(AudioReader&) override;
	};
}

#endif
