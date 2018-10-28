#include "backend.h"

#include "player.h"
#include "sound.h"

#include <comdef.h>

namespace Yttrium
{
	WasapiAudioBackend::WasapiAudioBackend()
	{
		ComPtr<IMMDeviceEnumerator> enumerator;
		::CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&enumerator));
		if (enumerator)
		{
			enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &_device);
			if (_device)
			{
				_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&_client));
				if (_client)
				{
					WAVEFORMATEX* format = nullptr;
					_client->GetMixFormat(&format);
					if (format)
					{
						_client->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10'000'000, 0, format, nullptr);
						::CoTaskMemFree(format);
					}
				}
			}
		}
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
