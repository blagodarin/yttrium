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

#include <cstring>

namespace Yttrium
{
	AudioMixer::AudioMixer(const AudioBackend::BufferInfo& buffer_info) noexcept
		: _buffer_info{ buffer_info }
	{
		switch (_buffer_info._format.sample_type())
		{
		case AudioSample::i16: _add_saturate = add_saturate_i16; break;
		case AudioSample::f32: _add_saturate = add_saturate_f32; break;
		}
	}

	void AudioMixer::mix(void* buffer)
	{
		auto out = buffer;
		if (_music)
		{
			if (!read(out, _conversion_buffer, *_music))
				_music.reset();
			else
				out = _mix_buffer.data();
		}
		if (_sound)
		{
			if (!read(out, _conversion_buffer, _sound->_reader))
				_sound.reset();
			else if (out != buffer)
				_add_saturate(buffer, out, _buffer_info._size / _buffer_info._format.bytes_per_sample());
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

	bool AudioMixer::read(void* out, Buffer& tmp, AudioReader& reader)
	{
		const auto format = reader.format();
		if (format == _buffer_info._format)
		{
			const auto out_bytes = reader.read(out, _buffer_info._size);
			if (!out_bytes)
				return false;
			if (out_bytes < _buffer_info._size)
				std::memset(static_cast<std::byte*>(out) + out_bytes, 0, _buffer_info._size - out_bytes);
		}
		else
		{
			tmp.reset(_buffer_info._size / _buffer_info._format.bytes_per_frame() * format.bytes_per_frame());
			const auto tmp_bytes = reader.read(tmp.data(), tmp.size());
			if (!tmp_bytes)
				return false;
			const auto frames = tmp_bytes / format.bytes_per_frame();
			if (!transform_audio(out, _buffer_info._format, tmp.data(), format, frames))
				return false;
			const auto out_bytes = frames * _buffer_info._format.bytes_per_frame();
			if (out_bytes < _buffer_info._size)
				std::memset(static_cast<std::byte*>(out) + out_bytes, 0, _buffer_info._size - out_bytes);
		}
		return true;
	}
}
