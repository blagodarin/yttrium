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

#include "writer.h"

#include <yttrium/memory/buffer.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/temporary_file.h>
#include "file.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <limits>

namespace Yttrium
{
	BufferWriter::BufferWriter(Buffer& buffer) noexcept
		: _buffer{ buffer }
	{
		_buffer.resize(0);
	}

	void BufferWriter::reserve(uint64_t size)
	{
		if (size > std::numeric_limits<size_t>::max())
			throw std::bad_alloc{};
		_buffer.reserve(static_cast<size_t>(size));
	}

	void BufferWriter::resize(uint64_t size)
	{
		if (size > std::numeric_limits<size_t>::max())
			throw std::bad_alloc{};
		_buffer.resize(static_cast<size_t>(size));
	}

	size_t BufferWriter::write_at(uint64_t offset, const void* data, size_t size)
	{
		assert(offset <= std::numeric_limits<size_t>::max());
		if (size > std::numeric_limits<size_t>::max() - offset)
			throw std::bad_alloc{};
		const auto required_size = static_cast<size_t>(offset + size);
		if (required_size > _buffer.size())
			_buffer.resize(required_size);
		std::memcpy(_buffer.begin() + offset, data, size);
		return size;
	}

	Writer::Writer(Buffer& buffer)
		: _private{ std::make_unique<BufferWriter>(buffer) }
	{
	}

	Writer::Writer(const std::string& path)
		: _private{ create_file_writer(path) }
	{
	}

	Writer::Writer(TemporaryFile& file)
		: _private{ create_file_writer(file) }
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
		_private->_size = std::max(_private->_size, _private->_offset);
		return result;
	}

	bool Writer::write_all(const Buffer& buffer)
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

	bool Writer::write_all(std::string_view string)
	{
		return write(string.data(), string.size()) == string.size();
	}

	size_t Writer::write_at(uint64_t offset, const void* data, size_t size)
	{
		if (!_private || offset > _private->_size)
			return 0;
		const auto result = _private->write_at(offset, data, size);
		if (result > 0)
			_private->_size = std::max(_private->_size, offset + result);
		return result;
	}

	Writer::Writer() noexcept = default;
	Writer::Writer(Writer&&) noexcept = default;
	Writer::~Writer() = default;
	Writer& Writer::operator=(Writer&&) noexcept = default;
}
