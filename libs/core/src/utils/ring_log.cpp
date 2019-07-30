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

#include "ring_log.h"

#include <yttrium/utils/numeric.h>

#include <algorithm>
#include <cstring>

namespace
{
	static_assert(Yttrium::is_power_of_2(Yttrium::RingLog::BufferSize));

	// If BufferSize is a power of two, we can wrap offsets using masking.
	constexpr auto OffsetMask = Yttrium::RingLog::BufferSize - 1;
}

namespace Yttrium
{
	bool RingLog::pop(std::string& text)
	{
		if (_size == 0)
			return false;
		const auto text_offset = (_offset + 1) & OffsetMask;
		const auto text_size = _buffer[_offset];
		_offset = (text_offset + text_size) & OffsetMask;
		_size -= text_size + 1u;
		if (const auto continuous_size = _buffer.size() - text_offset; text_size > continuous_size)
		{
			text.assign(reinterpret_cast<const char*>(_buffer.data() + text_offset), continuous_size);
			text.append(reinterpret_cast<const char*>(_buffer.data()), text_size - continuous_size);
		}
		else
			text.assign(reinterpret_cast<const char*>(_buffer.data() + text_offset), text_size);
		return true;
	}

	void RingLog::push(std::string_view text) noexcept
	{
		const auto text_size = std::min(text.size(), MaxStringSize);
		while (text_size >= _buffer.size() - _size)
		{
			const auto skip = 1u + _buffer[_offset];
			_offset = (_offset + skip) & OffsetMask;
			_size -= skip;
		}
		_buffer[(_offset + _size++) & OffsetMask] = static_cast<uint8_t>(text_size);
		const auto begin = (_offset + _size) & OffsetMask;
		if (const auto continuous_size = _buffer.size() - begin; text_size > continuous_size)
		{
			std::memcpy(_buffer.data() + begin, text.data(), continuous_size);
			std::memcpy(_buffer.data(), text.data() + continuous_size, text_size - continuous_size);
		}
		else
			std::memcpy(_buffer.data() + begin, text.data(), text_size);
		_size += text_size;
	}
}
