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

#include <yttrium/audio/format.h>

#include <atomic>
#include <memory>

namespace Yttrium
{
	class AudioBackend
	{
	public:
		struct BufferInfo
		{
			AudioFormat _format;
			size_t _size = 0;
		};

		struct ThreadContext
		{
			virtual ~ThreadContext() noexcept = default;
		};

		static std::unique_ptr<AudioBackend> create(unsigned frames_per_second);

		virtual ~AudioBackend() = default;

		virtual BufferInfo buffer_info() const noexcept = 0;
		virtual std::unique_ptr<ThreadContext> create_thread_context() { return std::make_unique<ThreadContext>(); }
		virtual void flush() = 0;
		virtual bool write_buffer(const uint8_t* data, const std::atomic<bool>& interrupt) = 0;
	};
}
