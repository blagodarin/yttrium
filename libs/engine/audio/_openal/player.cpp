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

#include "player.h"

namespace Yttrium
{
	OpenALPlayer::OpenALPlayer()
	{
		::alSourcei(_source, AL_SOURCE_RELATIVE, AL_TRUE);
	}

	void OpenALPlayer::set_format(const AudioFormat& format)
	{
		_format = OpenALFormat(format);
	}

	void OpenALPlayer::fill_buffer(size_t index, void* data, size_t size)
	{
		::alBufferData(_buffers[index], _format._format, data, static_cast<ALsizei>(size), _format._frequency);
		::alSourceQueueBuffers(_source, 1, &_buffers[index]);
	}

	size_t OpenALPlayer::check_buffers()
	{
		ALint buffers_processed = 0;
		::alGetSourcei(_source, AL_BUFFERS_PROCESSED, &buffers_processed);
		return static_cast<size_t>(buffers_processed);
	}

	void OpenALPlayer::refill_buffer(void* data, size_t size)
	{
		ALuint buffer = 0;
		::alSourceUnqueueBuffers(_source, 1, &buffer);
		::alBufferData(buffer, _format._format, data, static_cast<ALsizei>(size), _format._frequency);
		::alSourceQueueBuffers(_source, 1, &buffer);
	}

	void OpenALPlayer::play()
	{
		::alSourcePlay(_source);
	}

	void OpenALPlayer::pause()
	{
		::alSourcePause(_source);
	}

	void OpenALPlayer::stop() noexcept
	{
		::alSourceStop(_source);

		ALint buffers_processed = 0;
		::alGetSourcei(_source, AL_BUFFERS_PROCESSED, &buffers_processed);

		ALuint buffers[2];
		::alSourceUnqueueBuffers(_source, buffers_processed, buffers);
	}
}
