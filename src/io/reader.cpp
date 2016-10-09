#include <yttrium/io/reader.h>
#include "reader.h"

#include <yttrium/memory/buffer.h>
#include <yttrium/string.h>
#include <yttrium/utils.h>

#include <cstring>
#include <limits>

namespace Yttrium
{
	ReaderBuffer::ReaderBuffer(const std::shared_ptr<const Buffer>& buffer)
		: ReaderPrivate(buffer->size())
		, _buffer(buffer)
	{
	}

	size_t ReaderBuffer::read_at(uint64_t offset, void* buffer, size_t size) const
	{
		std::memcpy(buffer, static_cast<const uint8_t*>(_buffer->data()) + offset, size);
		return size;
	}

	ReaderFile::ReaderFile(File&& file)
		: ReaderPrivate(file.size())
		, _file(std::move(file))
	{
	}

	size_t ReaderFile::read_at(uint64_t offset, void* buffer, size_t size) const
	{
		return _file.seek(offset) ? _file.read(buffer, size) : 0;
	}

	Reader::Reader(const std::shared_ptr<const Buffer>& buffer)
		: _private(buffer ? std::make_unique<ReaderBuffer>(buffer) : nullptr)
	{
	}

	Reader::Reader(File&& file)
		: _private(file ? std::make_unique<ReaderFile>(std::move(file)) : nullptr)
	{
	}

	uint64_t Reader::offset() const
	{
		return _private ? _private->_offset : 0;
	}

	size_t Reader::read(void* buffer, size_t size)
	{
		if (!size || !_private)
			return 0;
		if (_private->_offset == _private->_size)
			return 0;
		const auto result = _private->read_at(_private->_offset, buffer, min<uint64_t>(size, _private->_size - _private->_offset));
		_private->_offset += result;
		return result;
	}

	bool Reader::read_all(Buffer& buffer) const
	{
		if (!_private)
			return false;
		if (_private->_size > std::numeric_limits<size_t>::max())
			throw std::bad_alloc();
		buffer.reset(_private->_size);
		return _private->read_at(0, buffer.data(), buffer.size()) == buffer.size();
	}

	bool Reader::read_all(String& string) const
	{
		if (!_private)
			return false;
		if (_private->_size > std::numeric_limits<size_t>::max() - 1) // One extra byte for zero terminator. // TODO: Fix.
			throw std::bad_alloc();
		string.resize(_private->_size);
		return _private->read_at(0, string.text(), string.size()) == string.size();
	}

	size_t Reader::read_at(uint64_t offset, void* buffer, size_t size) const
	{
		if (!size || !_private || offset >= _private->_size)
			return 0;
		return _private->read_at(offset, buffer, min<uint64_t>(size, _private->_size - offset));
	}

	bool Reader::seek(uint64_t offset)
	{
		if (!_private || offset > _private->_size)
			return false;
		_private->_offset = offset;
		return true;
	}

	uint64_t Reader::size() const
	{
		return _private ? _private->_size : 0;
	}

	bool Reader::skip(uint64_t size)
	{
		if (!_private || size > _private->_size - _private->_offset)
			return false;
		_private->_offset += size;
		return true;
	}

	Reader::Reader() noexcept = default;
	Reader::Reader(Reader&&) noexcept = default;
	Reader::~Reader() = default;
	Reader& Reader::operator=(Reader&&) noexcept = default;
}
