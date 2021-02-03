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

#pragma once

#include <yttrium/audio/manager.h>

#include <aulosplay/player.hpp>

namespace Yt
{
	class AudioMixer;

	class AudioManagerImpl final
		: public AudioManager
		, private aulosplay::PlayerCallbacks
	{
	public:
		explicit AudioManagerImpl();
		~AudioManagerImpl() noexcept override;

		std::shared_ptr<Sound> create_sound(std::unique_ptr<Source>&&) override;
		void play_music(const std::shared_ptr<AudioReader>&) override;
		void play_sound(const std::shared_ptr<Sound>&) override;

	private:
		void onPlaybackError(aulosplay::PlaybackError) override;
		void onPlaybackError(std::string&&) override;
		void onPlaybackStarted() override {}
		void onPlaybackStopped() override {}

	private:
		const std::shared_ptr<AudioMixer> _mixer;
		std::unique_ptr<aulosplay::Player> _player;
	};
}
