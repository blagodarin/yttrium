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

#include <cstring>

namespace Yttrium
{
	const uint8_t* AudioMixer::mix_buffer()
	{
		if (!_music)
			return nullptr;
		const auto bytes_read = _music->read(_buffer.data(), _buffer_info._size);
		if (!bytes_read)
		{
			_music.reset();
			return nullptr;
		}
		if (bytes_read < _buffer_info._size)
			std::memset(_buffer.begin() + bytes_read, 0, _buffer_info._size - bytes_read);
		return _buffer.begin();
	}

	void AudioMixer::play_music(const std::shared_ptr<AudioReader>& music)
	{
		if (music == _music)
			return;
		if (music)
		{
			if (music->format() != _buffer_info._format)
				return;
			music->seek(0);
		}
		_music = music;
	}

	void AudioMixer::play_sound(const std::shared_ptr<Sound>&)
	{
	}
}
