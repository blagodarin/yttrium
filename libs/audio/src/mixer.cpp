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

#include "mixer.h"

#include <yttrium/audio/reader.h>
#include <yttrium/audio/utils.h>
#include "sound.h"

#include <algorithm>
#include <cassert>
#include <cstring>

namespace
{
	void add_saturate_f32(void* dst, const void* src, size_t count) noexcept
	{
		assert(count * sizeof(float) % Yttrium::AudioBackend::BlockAlignment == 0);
		for (size_t i = 0; i < count; ++i)
			static_cast<float*>(dst)[i] = std::clamp(static_cast<float*>(dst)[i] + static_cast<const float*>(src)[i], -1.f, 1.f);
	}
}

namespace Yttrium
{
	AudioMixer::AudioMixer(const AudioFormat& format)
		: _format{ format }
	{
		assert(_format.sample_type() == AudioSample::f32);
	}

	void AudioMixer::mix(const AudioBackend::BufferView& buffer)
	{
		assert(buffer._frames * _format.bytes_per_frame() % AudioBackend::BlockAlignment == 0);
		auto out = buffer._data;
		if (_music)
		{
			if (!read(out, buffer._frames, _conversion_buffer, *_music))
				_music.reset();
			else
			{
				_mix_buffer.reset(buffer._frames * _format.bytes_per_frame());
				out = _mix_buffer.data();
			}
		}
		if (_sound)
		{
			if (!read(out, buffer._frames, _conversion_buffer, _sound->_reader))
				_sound.reset();
			else if (out != buffer._data)
				add_saturate_f32(buffer._data, out, buffer._frames * _format.channels());
		}
	}

	void AudioMixer::play_music(const std::shared_ptr<AudioReader>& music)
	{
		if (music == _music)
			return;
		if (music)
			music->seek_frame(0);
		_music = music;
	}

	void AudioMixer::play_sound(const std::shared_ptr<Sound>& sound)
	{
		_sound = std::static_pointer_cast<SoundImpl>(sound);
		if (_sound)
			_sound->_reader.seek_frame(0);
	}

	bool AudioMixer::read(void* out, size_t out_frames, Buffer& in_buffer, AudioReader& reader)
	{
		if (const auto in_format = reader.format(); in_format == _format)
		{
			const auto in_frames = reader.read_frames(out, out_frames);
			if (!in_frames)
				return false;
			if (in_frames < out_frames)
				std::memset(static_cast<std::byte*>(out) + in_frames * _format.bytes_per_frame(), 0, (out_frames - in_frames) * _format.bytes_per_frame());
		}
		else
		{
			in_buffer.reset(out_frames * in_format.bytes_per_frame());
			const auto in_frames = reader.read_frames(in_buffer.data(), out_frames);
			if (!in_frames)
				return false;
			if (!transform_audio(out, _format, in_buffer.data(), in_format, in_frames))
				return false;
			if (in_frames < out_frames)
				std::memset(static_cast<std::byte*>(out) + in_frames * _format.bytes_per_frame(), 0, (out_frames - in_frames) * _format.bytes_per_frame());
		}
		return true;
	}
}