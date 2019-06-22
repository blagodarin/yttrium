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

#include <yttrium/memory/buffer.h>
#include "backend.h"

namespace Yttrium
{
	class AudioReader;
	class Sound;
	class SoundImpl;

	class AudioMixer
	{
	public:
		explicit AudioMixer(const AudioBackend::BufferInfo&) noexcept;

		bool empty() const noexcept { return !(_music || _sound); }
		void mix(void* buffer);
		void play_music(const std::shared_ptr<AudioReader>&);
		void play_sound(const std::shared_ptr<Sound>&);

	private:
		bool read(void* out, Buffer& tmp, AudioReader&);

	private:
		using AddSaturate = void (*)(void*, const void*, size_t);

		const AudioBackend::BufferInfo _buffer_info;
		Buffer _mix_buffer{ _buffer_info._size };
		Buffer _conversion_buffer{ _buffer_info._size };
		AddSaturate _add_saturate = [](void*, const void*, size_t) {};
		std::shared_ptr<AudioReader> _music;
		std::shared_ptr<SoundImpl> _sound;
	};
}
