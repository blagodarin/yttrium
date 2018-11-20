//
// Copyright 2018 Sergei Blagodarin
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

#include "streamer.h"

#include <yttrium/audio/format.h>
#include "backend.h"
#include "music_reader.h"

namespace Yttrium
{
	AudioStreamer::AudioStreamer(AudioPlayerBackend& backend, MusicReaderImpl& music)
		: _backend{ backend }
		, _music{ music }
		, _buffer{ _music.buffer_size() }
	{
		_backend.set_format(_music.format());
	}

	AudioStreamer::~AudioStreamer() noexcept
	{
		_backend.stop();
	}

	bool AudioStreamer::play_first()
	{
		_music.seek_start();
		for (size_t i = 0; i < AudioPlayerBackend::NumBuffers; ++i)
		{
			if (const auto size = _music.read(_buffer.data()); size > 0)
				_backend.fill_buffer(i, _buffer.data(), size);
			else if (i > 0)
				break;
			else
				return false;
		}
		_backend.play();
		return true;
	}

	bool AudioStreamer::play_next()
	{
		const auto free_buffers = _backend.check_buffers();
		for (auto i = free_buffers; i > 0; --i)
		{
			const auto size = _music.read(_buffer.data());
			if (!size)
				return i != free_buffers; // TODO: Return 'false' only when the playback is finished.
			_backend.refill_buffer(_buffer.data(), size);
		}
		return true;
	}
}
