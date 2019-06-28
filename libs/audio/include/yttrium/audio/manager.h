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

#pragma once

#include <yttrium/audio/api.h>

#include <memory>

namespace Yttrium
{
	class AudioReader;
	class Sound;
	class Source;

	/// Audio manager.
	class Y_AUDIO_API AudioManager
	{
	public:
		///
		static std::shared_ptr<AudioManager> create();

		virtual ~AudioManager() noexcept = default;

		///
		virtual std::shared_ptr<Sound> create_sound(std::unique_ptr<Source>&&) = 0;

		///
		virtual void play_music(const std::shared_ptr<AudioReader>&) = 0;

		///
		virtual void play_sound(const std::shared_ptr<Sound>&) = 0;
	};
}
