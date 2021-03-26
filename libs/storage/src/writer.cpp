// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/storage/writer.h>
#include "writer.h"

#include <yttrium/base/buffer.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary.h>
#include "platforms/file.h"

#include <cassert>
#include <cstring>
#include <limits>

namespace Yt
{
	BufferWriter::BufferWriter(Buffer& buffer) noexcept
		: _buffer{ buffer }
	{
		_buffer.clear();
	}

	bool BufferWriter::try_reserve(uint64_t size) noexcept
	{
		return size <= std::numeric_limits<size_t>::max()
			&& _buffer.try_reserve(static_cast<size_t>(size));
	}

	size_t BufferWriter::write_at(uint64_t offset64, const void* data, size_t size) noexcept
	{
		assert(offset64 <= std::numeric_limits<size_t>::max());
		const auto offset = static_cast<size_t>(offset64);
		if (const auto max_size = std::numeric_limits<size_t>::max() - offset; size > max_size)
			size = max_size;
		const auto required_size = offset + size;
		if (required_size > _buffer.size() && !_buffer.try_resize(required_size))
			size = _buffer.size() - offset;
		std::memcpy(_buffer.begin() + offset, data, size);
		return size;
	}

	Writer::Writer(Buffer& buffer)
		: _private{ std::make_unique<BufferWriter>(buffer) }
	{
	}

	Writer::Writer(const std::filesystem::path& path)
		: _private{ create_file_writer(path) }
	{
	}

	Writer::Writer(TemporaryFile& file)
		: _private{ create_file_writer(file) }
	{
	}

	uint64_t Writer::offset() const noexcept
	{
		return _private ? _private->_offset : 0;
	}

	bool Writer::try_reserve(uint64_t size) noexcept
	{
		return _private && _private->try_reserve(size);
	}

	bool Writer::seek(uint64_t offset) noexcept
	{
		if (!_private || offset > _private->_size)
			return false;
		_private->_offset = offset;
		return true;
	}

	uint64_t Writer::size() const noexcept
	{
		return _private ? _private->_size : 0;
	}

	size_t Writer::write(const void* data, size_t size) noexcept
	{
		if (!_private)
			return 0;
		const auto result = _private->write_at(_private->_offset, data, size);
		_private->_offset += result;
		if (_private->_size < _private->_offset)
			_private->_size = _private->_offset;
		return result;
	}

	bool Writer::write_all(const Buffer& buffer) noexcept
	{
		return write(buffer.data(), buffer.size()) == buffer.size();
	}

	bool Writer::write_all(const Source& source)
	{
		if (const auto data = source.data())
			return write_all(data, static_cast<size_t>(source.size()));
		uint64_t total_size = 0;
		Buffer buffer{ Buffer::memory_granularity() };
		while (auto size_read = source.read_at(total_size, buffer.data(), buffer.size()))
		{
			const auto size_written = write(buffer.data(), size_read);
			total_size += size_written;
			if (size_written < size_read)
				break;
		}
		return total_size == source.size();
	}

	Writer::Writer() noexcept = default;
	Writer::Writer(Writer&&) noexcept = default;
	Writer::~Writer() noexcept = default;
	Writer& Writer::operator=(Writer&&) noexcept = default;
}
