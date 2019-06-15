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

#ifndef _include_yttrium_audio_player_h_
#define _include_yttrium_audio_player_h_

#include <yttrium/api.h>

#include <memory>

namespace Yttrium
{
	class AudioManager;
	class AudioReader;

	/// Audio player.
	class Y_ENGINE_API AudioPlayer
	{
	public:
		///
		explicit AudioPlayer(AudioManager&);

		///
		~AudioPlayer() noexcept;

		///
		void set_music(const std::shared_ptr<AudioReader>&);

		///
		void set_paused(bool);

	private:
		const std::unique_ptr<class AudioPlayerPrivate> _private;
	};
}

#endif
