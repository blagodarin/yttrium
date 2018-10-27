#include "backend.h"

#include "player.h"
#include "sound.h"

#include <comdef.h>

namespace Yttrium
{
	WindowsAudioBackend::WindowsAudioBackend()
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

	WindowsAudioBackend::~WindowsAudioBackend() = default;

	std::unique_ptr<AudioPlayerBackend> WindowsAudioBackend::create_player()
	{
		return std::make_unique<WindowsAudioPlayer>();
	}

	std::unique_ptr<Sound> WindowsAudioBackend::create_sound(AudioReader& reader)
	{
		return std::make_unique<WindowsSound>(reader);
	}

	std::unique_ptr<AudioBackend> AudioBackend::create()
	{
		return std::make_unique<WindowsAudioBackend>();
	}
}
