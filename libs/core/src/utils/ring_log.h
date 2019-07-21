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

#include <yttrium/api.h>

#include <array>
#include <mutex>
#include <string_view>

namespace Yttrium
{
	class Y_CORE_API RingLog
	{
	public:
		bool pop(std::string&);
		void push(std::string_view) noexcept;

		// For testing.
		size_t offset() const noexcept;
		size_t size() const noexcept;

	private:
		static constexpr size_t SizePower = 12; // 4096 bytes.
		static constexpr size_t Size = 1 << SizePower;
		static constexpr size_t OffsetMask = Size - 1;

		mutable std::mutex _mutex;
		size_t _offset = 0;
		size_t _size = 0;
		std::array<uint8_t, Size> _buffer;
	};

	extern RingLog _ring_log;
}
