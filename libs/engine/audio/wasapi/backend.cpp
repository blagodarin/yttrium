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

namespace
{
	class WasapiError : public Yttrium::BadCall
	{
	public:
		WasapiError(std::string_view function, HRESULT error)
			: BadCall{ "WASAPI", function, make_error_message(error) } {}

	private:
		static std::string make_error_message(HRESULT error)
		{
			std::string_view error_name;
			switch (error)
			{
			case AUDCLNT_E_NOT_INITIALIZED: error_name = "AUDCLNT_E_NOT_INITIALIZED"; break;
			case AUDCLNT_E_ALREADY_INITIALIZED: error_name = "AUDCLNT_E_ALREADY_INITIALIZED"; break;
			case AUDCLNT_E_WRONG_ENDPOINT_TYPE: error_name = "AUDCLNT_E_WRONG_ENDPOINT_TYPE"; break;
			case AUDCLNT_E_DEVICE_INVALIDATED: error_name = "AUDCLNT_E_DEVICE_INVALIDATED"; break;
			case AUDCLNT_E_NOT_STOPPED: error_name = "AUDCLNT_E_NOT_STOPPED"; break;
			case AUDCLNT_E_BUFFER_TOO_LARGE: error_name = "AUDCLNT_E_BUFFER_TOO_LARGE"; break;
			case AUDCLNT_E_OUT_OF_ORDER: error_name = "AUDCLNT_E_OUT_OF_ORDER"; break;
			case AUDCLNT_E_UNSUPPORTED_FORMAT: error_name = "AUDCLNT_E_UNSUPPORTED_FORMAT"; break;
			case AUDCLNT_E_INVALID_SIZE: error_name = "AUDCLNT_E_INVALID_SIZE"; break;
			case AUDCLNT_E_DEVICE_IN_USE: error_name = "AUDCLNT_E_DEVICE_IN_USE"; break;
			case AUDCLNT_E_BUFFER_OPERATION_PENDING: error_name = "AUDCLNT_E_BUFFER_OPERATION_PENDING"; break;
			case AUDCLNT_E_THREAD_NOT_REGISTERED: error_name = "AUDCLNT_E_THREAD_NOT_REGISTERED"; break;
			case AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED: error_name = "AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED"; break;
			case AUDCLNT_E_ENDPOINT_CREATE_FAILED: error_name = "AUDCLNT_E_ENDPOINT_CREATE_FAILED"; break;
			case AUDCLNT_E_SERVICE_NOT_RUNNING: error_name = "AUDCLNT_E_SERVICE_NOT_RUNNING"; break;
			case AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED: error_name = "AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED"; break;
			case AUDCLNT_E_EXCLUSIVE_MODE_ONLY: error_name = "AUDCLNT_E_EXCLUSIVE_MODE_ONLY"; break;
			case AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL: error_name = "AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL"; break;
			case AUDCLNT_E_EVENTHANDLE_NOT_SET: error_name = "AUDCLNT_E_EVENTHANDLE_NOT_SET"; break;
			case AUDCLNT_E_INCORRECT_BUFFER_SIZE: error_name = "AUDCLNT_E_INCORRECT_BUFFER_SIZE"; break;
			case AUDCLNT_E_BUFFER_SIZE_ERROR: error_name = "AUDCLNT_E_BUFFER_SIZE_ERROR"; break;
			case AUDCLNT_E_CPUUSAGE_EXCEEDED: error_name = "AUDCLNT_E_CPUUSAGE_EXCEEDED"; break;
			case AUDCLNT_E_BUFFER_ERROR: error_name = "AUDCLNT_E_BUFFER_ERROR"; break;
			case AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED: error_name = "AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED"; break;
			case AUDCLNT_E_INVALID_DEVICE_PERIOD: error_name = "AUDCLNT_E_INVALID_DEVICE_PERIOD"; break;
			case AUDCLNT_E_INVALID_STREAM_FLAG: error_name = "AUDCLNT_E_INVALID_STREAM_FLAG"; break;
			case AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE: error_name = "AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE"; break;
			case AUDCLNT_E_OUT_OF_OFFLOAD_RESOURCES: error_name = "AUDCLNT_E_OUT_OF_OFFLOAD_RESOURCES"; break;
			case AUDCLNT_E_OFFLOAD_MODE_ONLY: error_name = "AUDCLNT_E_OFFLOAD_MODE_ONLY"; break;
			case AUDCLNT_E_NONOFFLOAD_MODE_ONLY: error_name = "AUDCLNT_E_NONOFFLOAD_MODE_ONLY"; break;
			case AUDCLNT_E_RESOURCES_INVALIDATED: error_name = "AUDCLNT_E_RESOURCES_INVALIDATED"; break;
			case AUDCLNT_E_RAW_MODE_UNSUPPORTED: error_name = "AUDCLNT_E_RAW_MODE_UNSUPPORTED"; break;
			case AUDCLNT_E_ENGINE_PERIODICITY_LOCKED: error_name = "AUDCLNT_E_ENGINE_PERIODICITY_LOCKED"; break;
			case AUDCLNT_E_ENGINE_FORMAT_LOCKED: error_name = "AUDCLNT_E_ENGINE_FORMAT_LOCKED"; break;
			case AUDCLNT_E_HEADTRACKING_ENABLED: error_name = "AUDCLNT_E_HEADTRACKING_ENABLED"; break;
			case AUDCLNT_E_HEADTRACKING_UNSUPPORTED: error_name = "AUDCLNT_E_HEADTRACKING_UNSUPPORTED"; break;
			}
			return Yttrium::error_to_string(error, error_name);
		}
	};
}

namespace Yttrium
{
	WasapiAudioBackend::WasapiAudioBackend(unsigned frames_per_second)
	{
		ComPtr<IMMDeviceEnumerator> enumerator;
		auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&enumerator));
		if (!enumerator)
			throw WasapiError{ "CoCreateInstance", hr };

		hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &_device);
		if (!_device)
			throw WasapiError{ "IMMDeviceEnumerator::GetDefaultAudioEndpoint", hr };

		hr = _device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&_client));
		if (!_client)
			throw WasapiError{ "IMMDevice::Activate", hr };

		REFERENCE_TIME period;
		hr = _client->GetDevicePeriod(nullptr, &period);
		if (FAILED(hr))
			throw WasapiError{ "IAudioClient::GetDevicePeriod", hr };

		WAVEFORMATEX* format = nullptr;
		hr = _client->GetMixFormat(&format);
		if (!format)
			throw WasapiError{ "IAudioClient::GetMixFormat", hr };

		UniquePtr<WAVEFORMATEX, CoTaskMemFree> format_deleter{ format };

		if (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
		{
			const auto extensible = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(format);
			if (!IsEqualGUID(extensible->SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) || extensible->Format.wBitsPerSample != 32)
			{
				extensible->Format.wBitsPerSample = 32;
				extensible->Format.nBlockAlign = 4 * format->nChannels;
				extensible->Format.nAvgBytesPerSec = format->nBlockAlign * format->nSamplesPerSec;
				extensible->Samples.wValidBitsPerSample = 32;
				extensible->SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
			}
		}
		else if (format->wFormatTag != WAVE_FORMAT_IEEE_FLOAT || format->wBitsPerSample != 32)
		{
			format->wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
			format->wBitsPerSample = 32;
			format->nBlockAlign = 4 * format->nChannels;
			format->nAvgBytesPerSec = format->nBlockAlign * format->nSamplesPerSec;
		}

		DWORD stream_flags = 0;
		if (format->nSamplesPerSec != frames_per_second)
		{
			stream_flags = AUDCLNT_STREAMFLAGS_RATEADJUST;
			format->nSamplesPerSec = frames_per_second;
			format->nAvgBytesPerSec = format->nBlockAlign * format->nSamplesPerSec;
		}

		hr = _client->Initialize(AUDCLNT_SHAREMODE_SHARED, stream_flags, period, 0, format, nullptr);
		if (FAILED(hr))
			throw WasapiError{ "IAudioClient::Initialize", hr };

		UINT32 buffer_frames;
		hr = _client->GetBufferSize(&buffer_frames);
		if (FAILED(hr))
			throw WasapiError{ "IAudioClient::GetBufferSize", hr };

		hr = _client->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&_render_client));
		if (!_render_client)
			throw WasapiError{ "IAudioClient::GetService", hr };

		_buffer_frames = buffer_frames / 2;
		_buffer_info._format = { AudioSample::f32, format->nChannels, format->nSamplesPerSec };
		_buffer_info._size = _buffer_frames * _buffer_info._format.bytes_per_frame();
	}

	WasapiAudioBackend::~WasapiAudioBackend() = default;

	void WasapiAudioBackend::play_buffer()
	{
		if (FAILED(_unlock_error))
			throw WasapiError{ "IAudioRenderClient::ReleaseBuffer", _unlock_error };
	}

	void WasapiAudioBackend::begin_context()
	{
		if (const auto hr = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED); FAILED(hr))
			throw WasapiError{ "CoInitializeEx", hr };
	}

	void WasapiAudioBackend::end_context() noexcept
	{
		::CoUninitialize();
	}

	void* WasapiAudioBackend::lock_buffer()
	{
		BYTE* buffer;
		if (const auto hr = _render_client->GetBuffer(_buffer_frames, &buffer); FAILED(hr))
			throw WasapiError{ "IAudioRenderClient::GetBuffer", hr };
		return static_cast<void*>(buffer);
	}

	void WasapiAudioBackend::unlock_buffer(bool update) noexcept
	{
		_unlock_error = _render_client->ReleaseBuffer(update ? _buffer_frames : 0, 0);
	}

	std::unique_ptr<AudioBackend> AudioBackend::create(unsigned frames_per_second)
	{
		return std::make_unique<WasapiAudioBackend>(frames_per_second);
	}
}
