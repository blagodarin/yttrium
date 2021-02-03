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

#pragma once

#include <yttrium/audio/format.h>
#include <yttrium/memory/buffer.h>

#include <aulosplay/player.hpp>

#include <mutex>

namespace Yt
{
	class AudioReader;
	class Sound;
	class SoundImpl;

	class AudioMixer : public aulosplay::Source
	{
	public:
		explicit AudioMixer(const AudioFormat&);

		void play_music(const std::shared_ptr<AudioReader>&);
		void play_sound(const std::shared_ptr<Sound>&);

	private:
		bool isStereo() const noexcept override { return true; }
		size_t onRead(float* buffer, size_t maxFrames) noexcept override;

	private:
		bool read(void* out, size_t out_frames, Buffer& in_buffer, AudioReader&);

	private:
		const AudioFormat _format;
		Buffer _mix_buffer;
		Buffer _conversion_buffer;
		std::mutex _mutex;
		std::shared_ptr<AudioReader> _music;
		std::shared_ptr<SoundImpl> _sound;
	};
}
