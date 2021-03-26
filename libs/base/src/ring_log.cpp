// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "ring_log.h"

#include <yttrium/base/numeric.h>

#include <algorithm>
#include <cassert>
#include <cstring>

namespace
{
	static_assert(Yt::is_power_of_2(Yt::RingLog::BufferSize));

	// If BufferSize is a power of two, we can wrap offsets using masking.
	constexpr auto OffsetMask = Yt::RingLog::BufferSize - 1;
}

namespace Yt
{
	bool RingLog::pop(std::string& text)
	{
		if (_size == 0)
			return false;
		const auto text_offset = (_offset + 1) & OffsetMask;
		const auto text_size = size_t{ _buffer[_offset] };
		_offset = (text_offset + text_size) & OffsetMask;
		assert(_size >= text_size + 1);
		_size -= text_size + 1;
		if (const auto continuous_size = _buffer.size() - text_offset; continuous_size < text_size)
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
			const auto skip = 1 + size_t{ _buffer[_offset] };
			_offset = (_offset + skip) & OffsetMask;
			assert(_size >= skip);
			_size -= skip;
		}
		_buffer[(_offset + _size++) & OffsetMask] = static_cast<uint8_t>(text_size);
		const auto text_offset = (_offset + _size) & OffsetMask;
		if (const auto continuous_size = _buffer.size() - text_offset; continuous_size < text_size)
		{
			std::memcpy(_buffer.data() + text_offset, text.data(), continuous_size);
			std::memcpy(_buffer.data(), text.data() + continuous_size, text_size - continuous_size);
		}
		else
			std::memcpy(_buffer.data() + text_offset, text.data(), text_size);
		_size += text_size;
		assert(_size <= _buffer.size());
	}
}
