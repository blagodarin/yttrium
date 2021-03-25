// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/storage/reader.h>

#include <yttrium/memory/buffer.h>
#include <yttrium/storage/source.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <limits>

namespace Yt
{
	size_t Reader::read(void* data, size_t size)
	{
		const auto result = _source.read_at(_offset, data, size);
		_offset += result;
		return result;
	}

	bool Reader::read_line(std::string& string)
	{
		string.clear();

		const auto source_size = _source.size();
		assert(_offset <= source_size);
		if (_offset == source_size)
			return false;

		size_t newlines = 0;
		const auto read_block = [this, &string, &newlines](const char* begin, const char* end) {
			const auto newline = std::find_if(begin, end, [](char c) { return c == '\r' || c == '\n'; });
			string.append(begin, newline);
			if (newline == end)
				return false; // There may be more data.
			++newlines;
			if (*newline == '\r')
			{
				char maybe_n = '\0';
				if (_source.read_at(string.size() + 1, &maybe_n, 1) && maybe_n == '\n')
					++newlines;
			}
			return true;
		};

		if (const auto data = static_cast<const char*>(_source.data()))
			read_block(data + _offset, data + source_size);
		else
		{
			std::array<char, 32> buffer;
			for (;;)
			{
				const auto bytes_read = _source.read_at(_offset + string.size(), buffer.data(), buffer.size());
				if (read_block(buffer.data(), buffer.data() + bytes_read) || bytes_read < buffer.size())
					break;
			}
		}
		_offset += string.size() + newlines;
		return true;
	}

	bool Reader::seek(uint64_t offset) noexcept
	{
		if (offset > _source.size())
			return false;
		_offset = offset;
		return true;
	}

	uint64_t Reader::size() const noexcept
	{
		return _source.size();
	}

	bool Reader::skip(uint64_t bytes) noexcept
	{
		const auto source_size = _source.size();
		assert(_offset <= source_size);
		if (bytes > source_size - _offset)
			return false;
		_offset += bytes;
		return true;
	}
}
