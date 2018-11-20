#include <yttrium/storage/reader.h>

#include <yttrium/memory/buffer.h>
#include <yttrium/storage/source.h>

#include <algorithm>
#include <array>
#include <limits>

namespace Yttrium
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
		if (_offset == _source.size())
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
			read_block(data + _offset, data + _source.size());
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

	bool Reader::seek(uint64_t offset)
	{
		if (offset > _source.size())
			return false;
		_offset = offset;
		return true;
	}

	uint64_t Reader::size() const
	{
		return _source.size();
	}

	bool Reader::skip(uint64_t size)
	{
		if (size > _source.size() - _offset)
			return false;
		_offset += size;
		return true;
	}
}
