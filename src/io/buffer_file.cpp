#include "buffer_file.h"

#include <cstring>
#include <limits>
#include <stdexcept>

namespace Yttrium
{
	BufferFile::BufferFile(String&& name, unsigned mode, Buffer&& buffer)
		: FilePrivate(std::move(name), mode, buffer.size())
		, _buffer(std::move(buffer))
	{
	}

	BufferFile::~BufferFile() = default;

	bool BufferFile::flush()
	{
		return true;
	}

	size_t BufferFile::read(void*, size_t)
	{
		throw std::logic_error("Bad memory file operation");
	}

	size_t BufferFile::read(void* buffer, size_t size, uint64_t offset)
	{
		std::memcpy(buffer, static_cast<const uint8_t*>(_buffer.data()) + offset, size);
		return size;
	}

	bool BufferFile::resize(uint64_t size)
	{
		if (size > std::numeric_limits<size_t>::max())
			throw std::bad_alloc();
		_buffer.resize(size);
		return true;
	}

	size_t BufferFile::write(const void*, size_t)
	{
		throw std::logic_error("Bad memory file operation");
	}

	size_t BufferFile::write(const void* buffer, size_t size, uint64_t offset)
	{
		if (size > std::numeric_limits<uint64_t>::max() - offset || offset + size > std::numeric_limits<size_t>::max())
			throw std::bad_alloc();
		if (offset + size > _buffer.size())
			_buffer.resize(offset + size);
		std::memcpy(static_cast<uint8_t*>(_buffer.data()) + offset, buffer, size);
		return size;
	}
}
