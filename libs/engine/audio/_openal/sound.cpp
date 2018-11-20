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

#include "sound.h"

#include <yttrium/audio/format.h>
#include <yttrium/audio/reader.h>
#include <yttrium/memory/buffer.h>

#include <limits>
#include <new>

namespace Yttrium
{
	OpenALSound::OpenALSound(AudioReader& reader)
		: _format(reader.format())
	{
		if (reader.format().channels() != 1)
			throw DataError("Sound audio must have one channel");
		const auto reader_size = reader.total_bytes();
		if (reader_size > static_cast<std::remove_const_t<decltype(reader_size)>>(std::numeric_limits<ALsizei>::max()))
			throw std::bad_alloc();
		Buffer buffer(reader_size);
		if (reader.read(buffer.data(), buffer.size()) != buffer.size())
			throw DataError("Bad audio data");
		::alBufferData(_buffer, _format._format, buffer.data(), static_cast<ALsizei>(buffer.size()), _format._frequency);
		if (::alGetError() != AL_NO_ERROR)
			throw std::runtime_error("Failed to initialize a Sound");
		::alSourcei(_source, AL_BUFFER, static_cast<ALint>(_buffer)); // This must be done after alBufferData.
		::alSourcei(_source, AL_SOURCE_RELATIVE, AL_TRUE);
	}

	void OpenALSound::play() const
	{
		::alSourceStop(_source);
		::alSourcePlay(_source);
	}
}
