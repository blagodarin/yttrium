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

#include <cstddef>
#include <cstdint>

namespace Yttrium
{
	class Adler32
	{
	public:
		constexpr Adler32() noexcept = default;

		constexpr Adler32& process(const void* data, std::size_t size) noexcept
		{
			auto s1 = _value & 0xffff;
			auto s2 = _value >> 16;
			auto block_size = size % 5552;
			for (std::size_t i = 0; i < size;)
			{
				for (std::size_t j = 0; j < block_size; ++j)
				{
					s1 += static_cast<const std::uint8_t*>(data)[i + j];
					s2 += s1;
				}
				s1 %= 65521;
				s2 %= 65521;
				i += block_size;
				block_size = 5552;
			}
			_value = (s2 << 16) + s1;
			return *this;
		}

		constexpr std::uint32_t value() const noexcept
		{
			return _value;
		}

	private:
		std::uint32_t _value = 1;
	};
}
