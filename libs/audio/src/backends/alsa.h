//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include <yttrium/memory/buffer.h>
#include "../../../core/src/utils/memory.h"

#include <alsa/asoundlib.h>

namespace Yttrium
{
	class AlsaAudioBackend final : public AudioBackend
	{
	public:
		explicit AlsaAudioBackend(unsigned frames_per_second);
		~AlsaAudioBackend() override;

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
		SmartPtr<snd_pcm_t, snd_pcm_close> _pcm;
		snd_pcm_uframes_t _period_frames = 0;
		snd_pcm_uframes_t _buffer_frames = 0;
		Buffer _period;
	};
}
