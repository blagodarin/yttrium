// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/audio/manager.h>

#include <seir_audio/player.hpp>

namespace Yt
{
	class AudioMixer;

	class AudioManagerImpl final
		: public AudioManager
		, private seir::AudioCallbacks
	{
	public:
		explicit AudioManagerImpl();
		~AudioManagerImpl() noexcept override;

		std::shared_ptr<Sound> create_sound(std::unique_ptr<Source>&&) override;
		void play_music(const std::shared_ptr<AudioReader>&) override;
		void play_sound(const std::shared_ptr<Sound>&) override;

	private:
		void onPlaybackError(seir::AudioError) override;
		void onPlaybackError(std::string&&) override;
		void onPlaybackStarted() override {}
		void onPlaybackStopped() override {}

	private:
		const std::shared_ptr<AudioMixer> _mixer;
		std::unique_ptr<seir::AudioPlayer> _player;
	};
}
