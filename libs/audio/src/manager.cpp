// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "manager.h"

#include <yttrium/base/logger.h>
#include "mixer.h"
#include "sound.h"

namespace Yt
{
	AudioManagerImpl::AudioManagerImpl()
		: _mixer{ std::make_shared<AudioMixer>(AudioFormat{ AudioSample::f32, 2, 44'100 }) }
		, _player{ seir::AudioPlayer::create(*this, 44'100) }
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
		_mixer->playMusic(music);
	}

	void AudioManagerImpl::play_sound(const std::shared_ptr<Sound>& sound)
	{
		_mixer->playSound(sound);
	}

	void AudioManagerImpl::onPlaybackError(seir::AudioError error)
	{
		switch (error)
		{
		case seir::AudioError::NoDevice: Logger::write("(AudioManager) No audio device found"); break;
		}
	}

	void AudioManagerImpl::onPlaybackError(std::string&& message)
	{
		Logger::write(message.insert(0, "(AudioManager) "));
	}

	std::shared_ptr<AudioManager> AudioManager::create()
	{
		return std::make_shared<AudioManagerImpl>();
	}
}
