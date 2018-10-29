#ifndef _libs_yttrium_audio_wasapi_backend_h_
#define _libs_yttrium_audio_wasapi_backend_h_

#include "../backend.h"
#include "com.h"

#include <audioclient.h>
#include <mmdeviceapi.h>

namespace Yttrium
{
	class WasapiAudioBackend final : public AudioBackend
	{
	public:
		WasapiAudioBackend();
		~WasapiAudioBackend() override;

		std::unique_ptr<AudioPlayerBackend> create_player() override;
		std::unique_ptr<Sound> create_sound(AudioReader&) override;

	private:
		// MSDN: "In Windows 8, the first use of IAudioClient to access the audio device should
		// be on the STA thread. Calls from an MTA thread may result in undefined behavior."
		ComInitializer _com{COINIT_APARTMENTTHREADED};
		ComPtr<IMMDevice> _device;
		ComPtr<IAudioClient> _client;
	};
}

#endif