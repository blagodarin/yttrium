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

// TODO: Implement.

namespace Yttrium
{
	WasapiAudioPlayer::WasapiAudioPlayer()
	{
	}

	void WasapiAudioPlayer::set_format(const AudioFormat& format)
	{
		(void)format;
	}

	void WasapiAudioPlayer::fill_buffer(size_t index, void* data, size_t size)
	{
		(void)index;
		(void)data;
		(void)size;
	}

	size_t WasapiAudioPlayer::check_buffers()
	{
		return 0;
	}

	void WasapiAudioPlayer::refill_buffer(void* data, size_t size)
	{
		(void)data;
		(void)size;
	}

	void WasapiAudioPlayer::play()
	{
	}

	void WasapiAudioPlayer::pause()
	{
	}

	void WasapiAudioPlayer::stop() noexcept
	{
	}
}
