#include "writer.h"

#include <yttrium/memory/buffer.h>
#include <yttrium/storage/temporary_file.h>
#include <yttrium/utils.h>
#include "../system/file.h"
#include "reader.h"

#include <cassert>
#include <cstring>
#include <limits>

namespace Yttrium
{
	BufferWriter::BufferWriter(Buffer& buffer)
		: WriterPrivate(buffer.size())
		, _buffer(buffer)
	{
	}

	void BufferWriter::reserve(uint64_t size)
	{
		if (size > std::numeric_limits<size_t>::max())
			throw std::bad_alloc();
		_buffer.reserve(size);
	}

	void BufferWriter::resize(uint64_t size)
	{
		if (size > std::numeric_limits<size_t>::max())
			throw std::bad_alloc();
		_buffer.resize(size);
	}

	size_t BufferWriter::write_at(uint64_t offset, const void* data, size_t size)
	{
		assert(offset <= std::numeric_limits<size_t>::max());
		if (size > std::numeric_limits<size_t>::max() - offset)
			throw std::bad_alloc();
		if (offset + size > _buffer.size())
			_buffer.resize(offset + size);
		std::memcpy(_buffer.begin() + offset, data, size);
		return size;
	}

	Writer::Writer(Buffer& buffer)
		: _private(std::make_unique<BufferWriter>(buffer))
	{
	}

	Writer::Writer(const StaticString& path)
		: _private(create_file_writer(path.to_std()))
	{
	}

	Writer::Writer(TemporaryFile& file)
		: _private(create_file_writer(file))
	{
	}

	uint64_t Writer::offset() const
	{
		return _private ? _private->_offset : 0;
	}

	void Writer::reserve(uint64_t size)
	{
		if (_private)
			_private->reserve(size);
	}

	void Writer::resize(uint64_t size)
	{
		if (_private)
			_private->resize(size);
	}

	bool Writer::seek(uint64_t offset)
	{
		if (!_private || offset > _private->_size)
			return false;
		_private->_offset = offset;
		return true;
	}

	uint64_t Writer::size() const
	{
		return _private ? _private->_size : 0;
	}

	void Writer::unlink()
	{
		if (_private)
			_private->unlink();
	}

	size_t Writer::write(const void* data, size_t size)
	{
		if (!_private)
			return 0;
		const auto result = write_at(_private->_offset, data, size);
		_private->_offset += result;
		_private->_size = max(_private->_size, _private->_offset);
		return result;
	}

	bool Writer::write_all(const Buffer& buffer)
	{
		return write(buffer.data(), buffer.size()) == buffer.size();
	}

	bool Writer::write_all(const Reader& reader)
	{
		if (!reader)
			return false;
        if (const auto data = ReaderPrivate::data(reader))
			return write_all(data, reader.size());
		uint64_t total_size = 0;
		Buffer buffer(Buffer::memory_granularity());
		while (auto size_read = reader.read_at(total_size, buffer.data(), buffer.size()))
		{
			const auto size_written = write(buffer.data(), size_read);
			total_size += size_written;
			if (size_written < size_read)
				break;
		}
		return total_size == reader.size();
	}

	bool Writer::write_all(const StaticString& string)
	{
		return write(string.text(), string.size()) == string.size();
	}

	size_t Writer::write_at(uint64_t offset, const void* data, size_t size)
	{
		if (!_private || offset > _private->_size)
			return 0;
		const auto result = _private->write_at(offset, data, size);
		if (result > 0)
			_private->_size = max(_private->_size, offset + result);
		return result;
	}

	Writer::Writer() noexcept = default;
	Writer::Writer(Writer&&) noexcept = default;
	Writer::~Writer() = default;
	Writer& Writer::operator=(Writer&&) noexcept = default;
}
