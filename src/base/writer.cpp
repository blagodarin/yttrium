#include "writer.h"

#include <yttrium/file.h>
#include <yttrium/memory/buffer.h>

#include <cstring>

namespace Yttrium
{
	Writer<Buffer>::Writer(Buffer& buffer) noexcept
		: _buffer(buffer)
		, _offset(_buffer.size())
	{
	}

	void Writer<Buffer>::reserve(size_t size)
	{
		_buffer.reserve(_offset + size);
	}

	bool Writer<Buffer>::write(const void* data, size_t size)
	{
		const auto old_size = _buffer.size();
		_buffer.resize(old_size + size);
		std::memcpy(_buffer.begin() + old_size, data, size);
		return true;
	}

	bool Writer<File>::write(const void* data, size_t size)
	{
		return _file.write(data, size) == size;
	}
}
