//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include <exception>
#include <memory>

namespace Yttrium
{
	class AudioBackend
	{
	public:
		static constexpr unsigned BlockAlignment = 16;

		struct BufferView
		{
			void* _data;
			size_t _frames;
		};

		struct BufferLock
		{
			AudioBackend& _backend;
			const BufferView _buffer = _backend.lock_buffer();
			const int _exceptions = std::uncaught_exceptions();
			BufferLock(AudioBackend& backend)
				: _backend{ backend } {}
			~BufferLock() noexcept { _backend.unlock_buffer(_exceptions == std::uncaught_exceptions()); }
		};

		struct Context
		{
			AudioBackend& _backend;
			Context(AudioBackend& backend)
				: _backend{ backend } { _backend.begin_context(); }
			~Context() noexcept { _backend.end_context(); }
		};

		static std::unique_ptr<AudioBackend> create(unsigned frames_per_second);

		virtual ~AudioBackend() = default;

		virtual AudioFormat buffer_format() const noexcept = 0;
		virtual void play_buffer() = 0;

	protected:
		virtual void begin_context() = 0;
		virtual void end_context() noexcept = 0;
		virtual BufferView lock_buffer() = 0;
		virtual void unlock_buffer(bool update) noexcept = 0;
	};
}
