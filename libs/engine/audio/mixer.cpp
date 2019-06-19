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
	const uint8_t* AudioMixer::mix_buffer()
	{
		Buffer* out = &_buffer;
		if (_music)
		{
			if (read(*out, _conversion_buffer, *_music))
				out = &_mix_buffer;
			else
				_music.reset();
		}
		if (_sound)
		{
			if (read(*out, _conversion_buffer, _sound->_reader))
			{
				if (out == &_mix_buffer)
					add_saturate_i16(_buffer.data(), _mix_buffer.data(), _buffer_info._size / _buffer_info._format.bytes_per_sample());
				else
					out = &_mix_buffer;
			}
			else
				_sound.reset();
		}
		return out == &_mix_buffer ? _buffer.begin() : nullptr;
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

	bool AudioMixer::read(Buffer& out, Buffer& tmp, AudioReader& reader)
	{
		const auto format = reader.format();
		if (format == _buffer_info._format)
		{
			const auto out_bytes = reader.read(out.data(), out.size());
			if (!out_bytes)
				return false;
			if (out_bytes < out.size())
				std::memset(out.begin() + out_bytes, 0, out.size() - out_bytes);
		}
		else
		{
			tmp.reset(_buffer_info._size / _buffer_info._format.frame_bytes() * format.frame_bytes());
			const auto tmp_bytes = reader.read(tmp.data(), tmp.size());
			if (!tmp_bytes)
				return false;
			const auto frames = tmp_bytes / format.frame_bytes();
			if (!transform_audio(frames, format.bytes_per_sample(), format.channels(), tmp.data(), _buffer_info._format.bytes_per_sample(), _buffer_info._format.channels(), out.data()))
				return false;
			const auto out_bytes = frames * _buffer_info._format.frame_bytes();
			if (out_bytes < out.size())
				std::memset(out.begin() + out_bytes, 0, out.size() - out_bytes);
		}
		return true;
	}
}
