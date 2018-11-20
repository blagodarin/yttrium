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

#include <yttrium/audio/manager.h>

#include <yttrium/audio/reader.h>
#include <yttrium/audio/sound.h>
#include "backend.h"
#include "manager.h"
#include "player.h"

namespace Yttrium
{
	AudioManagerPrivate::AudioManagerPrivate()
		: _backend{ AudioBackend::create() }
	{
	}

	AudioManager::AudioManager()
		: _private{ std::make_unique<AudioManagerPrivate>() }
	{
	}

	AudioManager::~AudioManager() noexcept = default;

	std::unique_ptr<Sound> AudioManager::create_sound(std::unique_ptr<Source>&& source)
	{
		const auto reader = AudioReader::open(std::move(source));
		return reader ? _private->_backend->create_sound(*reader) : nullptr;
	}
}
