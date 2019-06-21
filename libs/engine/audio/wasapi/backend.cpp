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
#include "../../../core/utils/memory.h"
#include "../../application/_windows/error.h"

namespace Yttrium
{
	WasapiAudioBackend::WasapiAudioBackend()
	{
		ComPtr<IMMDeviceEnumerator> enumerator;
		auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&enumerator));
		if (!enumerator)
			throw BadCall{ "WASAPI", "CoCreateInstance", error_to_string(hr) };

		hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &_device);
		if (!_device)
			throw BadCall{ "WASAPI", "IMMDeviceEnumerator::GetDefaultAudioEndpoint", error_to_string(hr) };

		hr = _device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&_client));
		if (!_client)
			throw BadCall{ "WASAPI", "IMMDevice::Activate", error_to_string(hr) };

		REFERENCE_TIME period;
		hr = _client->GetDevicePeriod(nullptr, &period);
		if (FAILED(hr))
			throw BadCall{ "WASAPI", "IAudioClient::GetDevicePeriod", error_to_string(hr) };

		WAVEFORMATEX* format = nullptr;
		hr = _client->GetMixFormat(&format);
		if (!format)
			throw BadCall{ "WASAPI", "IAudioClient::GetMixFormat", error_to_string(hr) };

		UniquePtr<WAVEFORMATEX, CoTaskMemFree> format_deleter{ format };

		const auto sample_type = [format]() {
			if (format->wFormatTag == WAVE_FORMAT_PCM)
			{
				if (format->wBitsPerSample == 16)
					return AudioSample::i16;
			}
			else if (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
			{
				const auto extensible = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(format);
				if (IsEqualGUID(extensible->SubFormat, KSDATAFORMAT_SUBTYPE_PCM))
				{
					if (format->wBitsPerSample == 16)
						return AudioSample::i16;
				}
				else if (IsEqualGUID(extensible->SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
				{
					if (format->wBitsPerSample == 32)
						return AudioSample::f32;
				}
			}
			throw std::runtime_error{ "Unsupported audio buffer format" };
		}();

		DWORD stream_flags = 0;
		if (format->nSamplesPerSec != 44'100)
		{
			stream_flags |= AUDCLNT_STREAMFLAGS_RATEADJUST;
			format->nSamplesPerSec = 44'100;
			format->nAvgBytesPerSec = format->nBlockAlign * format->nSamplesPerSec;
		}

		hr = _client->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, period, 0, format, nullptr);
		if (FAILED(hr))
			throw BadCall{ "WASAPI", "IAudioClient::Initialize", error_to_string(hr) };

		UINT32 buffer_frames;
		hr = _client->GetBufferSize(&buffer_frames);
		if (FAILED(hr))
			throw BadCall{ "WASAPI", "IAudioClient::GetBufferSize", error_to_string(hr) };

		hr = _client->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&_render_client));
		if (!_render_client)
			throw BadCall{ "WASAPI", "IAudioClient::GetService", error_to_string(hr) };

		_buffer_info._format = { sample_type, format->nChannels, format->nSamplesPerSec };
		_buffer_info._size = buffer_frames * _buffer_info._format.bytes_per_frame();
	}

	WasapiAudioBackend::~WasapiAudioBackend() = default;

	std::unique_ptr<AudioBackend::ThreadContext> WasapiAudioBackend::create_thread_context()
	{
		struct WasapiThreadContext final : public ThreadContext
		{
			ComInitializer _com{ COINIT_APARTMENTTHREADED };
		};

		return std::make_unique<WasapiThreadContext>();
	}

	std::unique_ptr<AudioBackend> AudioBackend::create()
	{
		return std::make_unique<WasapiAudioBackend>();
	}
}
