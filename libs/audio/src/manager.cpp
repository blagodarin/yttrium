//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#include "manager.h"

#include <yttrium/logger.h>
#include "mixer.h"
#include "sound.h"

namespace Yt
{
	AudioManagerImpl::AudioManagerImpl()
		: _mixer{ std::make_shared<AudioMixer>(AudioFormat{ AudioSample::f32, 2, 44'100 }) }
		, _player{ aulosplay::Player::create(*this, 44'100) }
	{
		_player->play(_mixer);
	}

	AudioManagerImpl::~AudioManagerImpl() noexcept
	{
		_player.reset(); // Prevent the player from calling our virtual functions during destruction.
	}

	std::shared_ptr<Sound> AudioManagerImpl::create_sound(std::unique_ptr<Source>&& source)
	{
		return source ? std::make_shared<SoundImpl>(std::move(source)) : nullptr;
	}

	void AudioManagerImpl::play_music(const std::shared_ptr<AudioReader>& music)
	{
		_mixer->play_music(music);
	}

	void AudioManagerImpl::play_sound(const std::shared_ptr<Sound>& sound)
	{
		_mixer->play_sound(sound);
	}

	void AudioManagerImpl::onPlaybackError(aulosplay::PlaybackError error)
	{
		switch (error)
		{
		case aulosplay::PlaybackError::NoDevice: Logger::log("(AudioManager) No audio device found"); break;
		}
	}

	void AudioManagerImpl::onPlaybackError(std::string&& message)
	{
		Logger::log("(AudioManager) ", message);
	}

	std::shared_ptr<AudioManager> AudioManager::create()
	{
		return std::make_shared<AudioManagerImpl>();
	}
}
