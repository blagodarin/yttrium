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

#pragma once

#include "../backend.h"
#include "format.h"
#include "wrappers.h"

#include <array>

namespace Yttrium
{
	class OpenALPlayer final : public AudioPlayerBackend
	{
	public:
		OpenALPlayer();

	private:
		void set_format(const AudioFormat&) override;
		void fill_buffer(size_t index, void* data, size_t size) override;
		size_t check_buffers() override;
		void refill_buffer(void* data, size_t size) override;
		void play() override;
		void pause() override;
		void stop() noexcept override;

	private:
		OpenALSource _source;
		std::array<OpenALBuffer, 2> _buffers;
		OpenALFormat _format;
	};
}
