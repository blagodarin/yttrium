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

#include "../../../core/utils/memory.h"
#include "../../application/openal.h"
#include "../backend.h"

namespace Yttrium
{
	using P_ALCdevice = UniquePtr<::ALCdevice, ::alcCloseDevice>;
	using P_ALCcontext = UniquePtr<::ALCcontext, ::alcDestroyContext>;

	class OpenALBackend final : public AudioBackend
	{
	public:
		OpenALBackend();
		~OpenALBackend() override;

		AudioFormat buffer_format() const noexcept override { return {}; }
		std::unique_ptr<AudioPlayerBackend> create_player() override;
		std::unique_ptr<Sound> create_sound(AudioReader&) override;
		bool write_buffer(const std::atomic<bool>&) override { return false; }

	private:
		const P_ALCdevice _device;
		const P_ALCcontext _context;
	};
}
