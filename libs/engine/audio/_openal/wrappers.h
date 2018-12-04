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

#pragma once

#include <yttrium/exceptions.h>
#include "../../application/openal.h"

#include <array>

namespace Yttrium
{
	template <ALsizei N>
	class OpenALBuffers
	{
	public:
		OpenALBuffers()
		{
			::alGenBuffers(N, _buffers.data());
			if (::alGetError() != AL_NO_ERROR)
				throw InitializationError{ "Failed to allocate OpenAL buffers" };
		}

		~OpenALBuffers()
		{
			::alDeleteBuffers(N, _buffers.data());
		}

		ALuint& operator[](std::size_t i) noexcept { return _buffers[i]; }

		OpenALBuffers(const OpenALBuffers&) = delete;
		OpenALBuffers& operator=(const OpenALBuffers&) = delete;

	private:
		std::array<ALuint, N> _buffers{};
	};

	class OpenALSource
	{
	public:
		OpenALSource()
		{
			::alGenSources(1, &_source);
			if (::alGetError() != AL_NO_ERROR)
				throw InitializationError("Failed to allocate OpenAL source");
		}

		~OpenALSource()
		{
			::alDeleteSources(1, &_source);
		}

		operator ALuint() const { return _source; }

		OpenALSource(const OpenALSource&) = delete;
		OpenALSource& operator=(const OpenALSource&) = delete;

	private:
		ALuint _source = 0;
	};
}
