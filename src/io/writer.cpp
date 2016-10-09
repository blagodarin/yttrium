#include "writer.h"

#include <yttrium/memory/buffer.h>
#include "file.h"

#include <cstring>
#include <limits>

namespace Yttrium
{
	BufferWriter::BufferWriter(Buffer& buffer)
		: _buffer(buffer)
		, _offset(_buffer.size())
	{
	}

	void BufferWriter::reserve(size_t size)
	{
		if (std::numeric_limits<size_t>::max() - _offset < size)
			throw std::bad_alloc();
		_buffer.reserve(_offset + size);
	}

	size_t BufferWriter::write(const void* data, size_t size)
	{
		if (std::numeric_limits<size_t>::max() - _offset < size)
			throw std::bad_alloc();
		_buffer.resize(_offset + size);
		std::memcpy(_buffer.begin() + _offset, data, size);
		_offset += size;
		return size;
	}

	size_t FileWriter::write(const void* data, size_t size)
	{
		return _file.write(data, size);
	}

	Writer::Writer(Buffer& buffer)
		: _private(std::make_unique<BufferWriter>(buffer))
	{
	}

	Writer::Writer(File& file)
		: _private(std::make_unique<FileWriter>(file))
	{
	}

	void Writer::reserve(size_t size)
	{
		if (_private)
			_private->reserve(size);
	}

	size_t Writer::write(const void* data, size_t size)
	{
		return _private ? _private->write(data, size) : 0;
	}

	uint64_t Writer::write_all(const File& file)
	{
		uint64_t total_size = 0;
		if (file._private && file._private->_mode & File::Read)
		{
			Buffer buffer(Buffer::memory_granularity());
			while (auto size_read = file._private->read(buffer.data(), buffer.size(), total_size))
			{
				const auto size_written = write(buffer.data(), size_read);
				total_size += size_written;
				if (size_written < size_read)
					break;
			}
		}
		return total_size;
	}

	Writer::Writer() noexcept = default;
	Writer::Writer(Writer&&) noexcept = default;
	Writer::~Writer() = default;
	Writer& Writer::operator=(Writer&&) noexcept = default;
}
