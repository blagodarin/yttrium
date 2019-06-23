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

#include "mixer.h"

#include <yttrium/audio/reader.h>
#include <yttrium/audio/utils.h>
#include "../../core/utils/processing.h"
#include "sound.h"

#include <cassert>
#include <cstring>

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
			music->seek(0);
		_music = music;
	}

	void AudioMixer::play_sound(const std::shared_ptr<Sound>& sound)
	{
		_sound = std::static_pointer_cast<SoundImpl>(sound);
		if (_sound)
			_sound->_reader.seek(0);
	}

	bool AudioMixer::read(void* out, size_t out_frames, Buffer& in_buffer, AudioReader& reader)
	{
		const auto out_bytes = out_frames * _format.bytes_per_frame();
		if (const auto in_format = reader.format(); in_format == _format)
		{
			const auto read_bytes = reader.read(out, out_bytes);
			if (!read_bytes)
				return false;
			if (read_bytes < out_bytes)
				std::memset(static_cast<std::byte*>(out) + read_bytes, 0, out_bytes - read_bytes);
		}
		else
		{
			in_buffer.reset(out_frames * in_format.bytes_per_frame());
			const auto in_bytes = reader.read(in_buffer.data(), in_buffer.size());
			if (!in_bytes)
				return false;
			const auto in_frames = in_bytes / in_format.bytes_per_frame();
			if (!transform_audio(out, _format, in_buffer.data(), in_format, in_frames))
				return false;
			const auto read_bytes = in_frames * _format.bytes_per_frame();
			if (read_bytes < out_bytes)
				std::memset(static_cast<std::byte*>(out) + read_bytes, 0, out_bytes - read_bytes);
		}
		return true;
	}
}
