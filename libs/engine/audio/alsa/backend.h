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

#include "../backend.h"

#include "../../../core/utils/memory.h"

#include <alsa/asoundlib.h>

namespace Yttrium
{
	class AlsaAudioBackend final : public AudioBackend
	{
	public:
		AlsaAudioBackend();
		~AlsaAudioBackend() override;

		BufferInfo buffer_info() const noexcept override;
		void flush() noexcept override;
		bool write_buffer(const uint8_t* data, const std::atomic<bool>&) noexcept override;

	private:
		UniquePtr<snd_pcm_t, snd_pcm_close> _pcm;
		snd_pcm_uframes_t _buffer_frames;
		snd_pcm_uframes_t _period_frames;
		size_t _period_bytes = 0;
	};
}
