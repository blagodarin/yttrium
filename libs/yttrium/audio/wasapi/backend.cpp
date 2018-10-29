#include "backend.h"

#include <yttrium/exceptions.h>
#include "com.h"
#include "player.h"
#include "sound.h"

namespace Yttrium
{
	WasapiAudioBackend::WasapiAudioBackend()
	{
		ComPtr<IMMDeviceEnumerator> enumerator;
		auto hr = ::CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&enumerator));
		if (!enumerator)
			throw InitializationError{"CoCreateInstance failed: ", hresult_to_string(hr)};

		hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &_device);
		if (!_device)
			throw InitializationError{"IMMDeviceEnumerator::GetDefaultAudioEndpoint failed: ", hresult_to_string(hr)};

		hr = _device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&_client));
		if (!_client)
			throw InitializationError{"IMMDevice::Activate failed: ", hresult_to_string(hr)};

		WAVEFORMATEX* format = nullptr;
		hr = _client->GetMixFormat(&format);
		if (!format)
			throw InitializationError{"IAudioClient::GetMixFormat failed: ", hresult_to_string(hr)};

		hr = _client->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10'000'000, 0, format, nullptr);
		::CoTaskMemFree(format);
		if (FAILED(hr))
			throw InitializationError{"IAudioClient::Initialize failed: ", hresult_to_string(hr)};
	}

	WasapiAudioBackend::~WasapiAudioBackend() = default;

	std::unique_ptr<AudioPlayerBackend> WasapiAudioBackend::create_player()
	{
		return std::make_unique<WasapiAudioPlayer>();
	}

	std::unique_ptr<Sound> WasapiAudioBackend::create_sound(AudioReader& reader)
	{
		return std::make_unique<WasapiSound>(reader);
	}

	std::unique_ptr<AudioBackend> AudioBackend::create()
	{
		return std::make_unique<WasapiAudioBackend>();
	}
}
