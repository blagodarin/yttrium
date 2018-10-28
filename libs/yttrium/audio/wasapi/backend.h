#ifndef _libs_yttrium_audio_wasapi_backend_h_
#define _libs_yttrium_audio_wasapi_backend_h_

#include "../backend.h"

#include <audioclient.h>
#include <comip.h>
#include <mmdeviceapi.h>

// MSDN: "In Windows 8, the first use of IAudioClient to access the audio device should
// be on the STA thread. Calls from an MTA thread may result in undefined behavior."

class ComInitializer
{
public:
	ComInitializer() noexcept : _initialized{ SUCCEEDED(::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)) } {}
	~ComInitializer() noexcept { if (_initialized) ::CoUninitialize(); }
private:;
	const bool _initialized;
};

template <typename T>
using ComPtr = _com_ptr_t<_com_IIID<T, &__uuidof(T)>>;

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
		ComInitializer _com;
		ComPtr<IMMDevice> _device;
		ComPtr<IAudioClient> _client;
	};
}

#endif
