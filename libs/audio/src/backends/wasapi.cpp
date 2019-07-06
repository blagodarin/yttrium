//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#include "wasapi.h"

#include <yttrium/exceptions.h>
#include <yttrium/utils/numeric.h>
#include "../../../core/src/platform/windows/error.h"

#include <numeric>

#include <comdef.h> // Required for linking COM facilities.

namespace
{
	constexpr unsigned AudioBufferChannels = 2;

	class WasapiError : public Yttrium::BadCall
	{
	public:
		WasapiError(std::string_view function, unsigned long error)
			: BadCall{ "WASAPI", function, make_error_message(error) } {}
		WasapiError(std::string_view function, long error)
			: WasapiError{ function, Yttrium::to_unsigned(error) } {}

	private:
		static std::string make_error_message(unsigned long error)
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
			}
			return Yttrium::error_to_string(error, error_name);
		}
	};
}

namespace Yttrium
{
	WasapiAudioBackend::WasapiAudioBackend(unsigned frames_per_second)
		: _buffer_format{ AudioSample::f32, AudioBufferChannels, frames_per_second }
		, _block_frames{ static_cast<UINT32>(std::lcm(BlockAlignment, _buffer_format.bytes_per_frame()) / _buffer_format.bytes_per_frame()) }
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

		SmartPtr<WAVEFORMATEX, CoTaskMemFree> format;
		hr = _client->GetMixFormat(format.out());
		if (!format)
			throw WasapiError{ "IAudioClient::GetMixFormat", hr };

		if (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
		{
			const auto extensible = format.get_as<WAVEFORMATEXTENSIBLE>();
			if (!IsEqualGUID(extensible->SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) || extensible->Format.wBitsPerSample != 32)
			{
				extensible->Format.wBitsPerSample = 32;
				extensible->Format.nBlockAlign = (format->wBitsPerSample / 8) * format->nChannels;
				extensible->Format.nAvgBytesPerSec = format->nBlockAlign * format->nSamplesPerSec;
				extensible->Samples.wValidBitsPerSample = 32;
				extensible->SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
			}
		}
		else if (format->wFormatTag != WAVE_FORMAT_IEEE_FLOAT || format->wBitsPerSample != 32)
		{
			format->wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
			format->wBitsPerSample = 32;
			format->nBlockAlign = (format->wBitsPerSample / 8) * format->nChannels;
			format->nAvgBytesPerSec = format->nBlockAlign * format->nSamplesPerSec;
		}

		DWORD stream_flags = AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
		if (format->nSamplesPerSec != frames_per_second)
		{
			stream_flags |= AUDCLNT_STREAMFLAGS_RATEADJUST;
			format->nSamplesPerSec = frames_per_second;
			format->nAvgBytesPerSec = format->nBlockAlign * format->nSamplesPerSec;
		}

		// This shouldn't work, but it should trigger the proper error.
		if (format->nChannels != AudioBufferChannels)
		{
			format->nChannels = AudioBufferChannels;
			format->nBlockAlign = (format->wBitsPerSample / 8) * format->nChannels;
			format->nAvgBytesPerSec = format->nBlockAlign * format->nSamplesPerSec;
		}

		hr = _client->Initialize(AUDCLNT_SHAREMODE_SHARED, stream_flags, period, 0, format, nullptr);
		if (FAILED(hr))
			throw WasapiError{ "IAudioClient::Initialize", hr };

		_event.reset(CreateEventW(nullptr, FALSE, FALSE, nullptr));
		if (!_event)
			throw WasapiError{ "CreateEventW", GetLastError() };

		hr = _client->SetEventHandle(_event.get());
		if (FAILED(hr))
			throw WasapiError{ "IAudioClient::SetEventHandle", hr };

		UINT32 buffer_frames;
		hr = _client->GetBufferSize(&buffer_frames);
		if (FAILED(hr))
			throw WasapiError{ "IAudioClient::GetBufferSize", hr };

		hr = _client->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&_render_client));
		if (!_render_client)
			throw WasapiError{ "IAudioClient::GetService", hr };

		_buffer_frames = buffer_frames;
		_update_frames = buffer_frames / _block_frames * _block_frames / 2;
	}

	WasapiAudioBackend::~WasapiAudioBackend() = default;

	void WasapiAudioBackend::play_buffer()
	{
		if (FAILED(_unlock_error))
			throw WasapiError{ "IAudioRenderClient::ReleaseBuffer", _unlock_error };

		if (!_started)
		{
			if (const auto hr = _client->Start(); FAILED(hr))
				throw WasapiError{ "IAudioClient::Start", hr };
			_started = true;
		}
	}

	void WasapiAudioBackend::begin_context()
	{
		if (const auto hr = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED); FAILED(hr))
			throw WasapiError{ "CoInitializeEx", hr };
	}

	void WasapiAudioBackend::end_context() noexcept
	{
		if (_started)
			_client->Stop();
		::CoUninitialize();
	}

	AudioBackend::BufferView WasapiAudioBackend::lock_buffer()
	{
		decltype(_locked_frames) locked_frames;
		for (;;)
		{
			decltype(_locked_frames) padding_frames;
			if (const auto status = _client->GetCurrentPadding(&padding_frames); FAILED(status))
				throw WasapiError{ "IAudioClient::GetCurrentPadding", status };
			locked_frames = (_buffer_frames - padding_frames) / _block_frames * _block_frames;
			if (locked_frames >= _update_frames)
				break;
			const auto padding_ms = padding_frames * 1000 / static_cast<DWORD>(_buffer_format.frames_per_second());
			if (const auto status = WaitForSingleObjectEx(_event.get(), 2 * padding_ms, FALSE); status != WAIT_OBJECT_0)
				throw WasapiError{ "WaitForSingleObjectEx", status == WAIT_TIMEOUT ? ERROR_TIMEOUT : GetLastError() };
		}

		BYTE* buffer;
		if (const auto hr = _render_client->GetBuffer(locked_frames, &buffer); FAILED(hr))
			throw WasapiError{ "IAudioRenderClient::GetBuffer", hr };

		_locked_frames = locked_frames;
		return { static_cast<void*>(buffer), _locked_frames };
	}

	void WasapiAudioBackend::unlock_buffer(bool update) noexcept
	{
		_unlock_error = _render_client->ReleaseBuffer(update ? _locked_frames : 0, 0);
		_locked_frames = 0;
	}

	std::unique_ptr<AudioBackend> AudioBackend::create(unsigned frames_per_second)
	{
		return std::make_unique<WasapiAudioBackend>(frames_per_second);
	}
}
