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

#pragma once

#include "../../../core/utils/memory.h"
#include "../../application/_windows/com.h"
#include "../backend.h"

#include <type_traits>

#include <audioclient.h>
#include <mmdeviceapi.h>

namespace Yttrium
{
	class WasapiAudioBackend final : public AudioBackend
	{
	public:
		explicit WasapiAudioBackend(unsigned frames_per_second);
		~WasapiAudioBackend() override;

		AudioFormat buffer_format() const noexcept override { return _buffer_format; }
		void play_buffer() override;

	private:
		void begin_context() override;
		void end_context() noexcept override;
		BufferView lock_buffer() override;
		void unlock_buffer(bool) noexcept override;

	private:
		const AudioFormat _buffer_format;
		const size_t _block_frames;
		// MSDN: "In Windows 8, the first use of IAudioClient to access the audio device should
		// be on the STA thread. Calls from an MTA thread may result in undefined behavior."
		ComInitializer _com{ COINIT_APARTMENTTHREADED };
		ComPtr<IMMDevice> _device;
		ComPtr<IAudioClient> _client;
		UniquePtr<std::remove_pointer_t<HANDLE>, CloseHandle> _event;
		ComPtr<IAudioRenderClient> _render_client;
		UINT32 _buffer_frames = 0;
		UINT32 _update_frames = 0;
		UINT32 _locked_frames = 0;
		HRESULT _unlock_error = S_OK;
		bool _started = false;
	};
}
