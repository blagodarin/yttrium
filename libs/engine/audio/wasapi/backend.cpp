//
// Copyright 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "backend.h"

#include <yttrium/exceptions.h>
#include "../../application/_windows/error.h"

namespace Yttrium
{
	WasapiAudioBackend::WasapiAudioBackend()
	{
		ComPtr<IMMDeviceEnumerator> enumerator;
		auto hr = ::CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&enumerator));
		if (!enumerator)
			throw InitializationError{ "CoCreateInstance failed: ", error_to_string(hr) };

		hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &_device);
		if (!_device)
			throw InitializationError{ "IMMDeviceEnumerator::GetDefaultAudioEndpoint failed: ", error_to_string(hr) };

		hr = _device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&_client));
		if (!_client)
			throw InitializationError{ "IMMDevice::Activate failed: ", error_to_string(hr) };

		WAVEFORMATEX* format = nullptr;
		hr = _client->GetMixFormat(&format);
		if (!format)
			throw InitializationError{ "IAudioClient::GetMixFormat failed: ", error_to_string(hr) };

		hr = _client->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 10'000'000, 0, format, nullptr);
		::CoTaskMemFree(format);
		if (FAILED(hr))
			throw InitializationError{ "IAudioClient::Initialize failed: ", error_to_string(hr) };
	}

	WasapiAudioBackend::~WasapiAudioBackend() = default;

	std::unique_ptr<AudioBackend> AudioBackend::create()
	{
		return std::make_unique<WasapiAudioBackend>();
	}
}
