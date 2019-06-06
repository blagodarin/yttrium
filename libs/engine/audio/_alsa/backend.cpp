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

#include "backend.h"

#include <yttrium/exceptions.h>
#include "player.h"
#include "sound.h"

namespace Yttrium
{
	AlsaAudioBackend::AlsaAudioBackend()
	{
		snd_pcm_t* pcm_handle = nullptr;
		if (const auto status = snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0); status < 0)
			throw InitializationError{ "snd_pcm_open failed: ", snd_strerror(status) };
		_pcm_handle.reset(pcm_handle);
	}

	AlsaAudioBackend::~AlsaAudioBackend() = default;

	std::unique_ptr<AudioPlayerBackend> AlsaAudioBackend::create_player()
	{
		return std::make_unique<AlsaAudioPlayer>();
	}

	std::unique_ptr<Sound> AlsaAudioBackend::create_sound(AudioReader& reader)
	{
		return std::make_unique<AlsaSound>(reader);
	}

	std::unique_ptr<AudioBackend> AudioBackend::create()
	{
		return std::make_unique<AlsaAudioBackend>();
	}
}
