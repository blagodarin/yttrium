#include "reader.h"

#include <yttrium/memory/buffer.h>
#include <yttrium/string.h>
#include <yttrium/utils.h>
#include "file.h"

#include <cstring>
#include <limits>

namespace Yttrium
{
	BufferReader::BufferReader(const std::shared_ptr<const Buffer>& buffer)
		: ReaderPrivate(buffer->size())
		, _buffer(buffer)
	{
	}

	size_t BufferReader::read_at(uint64_t offset, void* buffer, size_t size) const
	{
		std::memcpy(buffer, static_cast<const uint8_t*>(_buffer->data()) + offset, size);
		return size;
	}

	FileReader::FileReader(const SharedPtr<const FilePrivate>& file)
		: ReaderPrivate(file->size())
		, _file(file)
	{
	}

	size_t FileReader::read_at(uint64_t offset, void* buffer, size_t size) const
	{
		return _file->read(buffer, size, offset);
	}

	ReaderReader::ReaderReader(const std::shared_ptr<const ReaderPrivate>& reader, uint64_t base, uint64_t size)
		: ReaderPrivate(size)
		, _reader(reader)
		, _base(base)
	{
	}

	size_t ReaderReader::read_at(uint64_t offset, void* buffer, size_t size) const
	{
		return offset <= std::numeric_limits<uint64_t>::max() - _base ? _reader->read_at(_base + offset, buffer, size) : 0;
	}

	Reader::Reader(const std::shared_ptr<const Buffer>& buffer)
		: _private(buffer ? std::make_unique<BufferReader>(buffer) : nullptr)
	{
	}

	Reader::Reader(File&& file)
		: _private(file ? std::make_unique<FileReader>(std::move(file._private)) : nullptr)
	{
	}

	Reader::Reader(const StaticString& path, Allocator& allocator)
		: Reader(File(path, File::Read, allocator))
	{
	}

	Reader::Reader(const Reader& reader, uint64_t base, uint64_t size)
		: _private(base < reader.size() ? std::make_unique<ReaderReader>(reader._private, base, min(size, reader.size() - base)) : nullptr)
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
		if (_private->_size > std::numeric_limits<size_t>::max() - 1)
			throw std::bad_alloc();
		buffer.reset(_private->_size + 1);
		if (_private->read_at(0, buffer.data(), _private->_size) != _private->_size)
			return false;
		buffer[_private->_size] = '\0';
		buffer.resize(_private->_size);
		return true;
	}

	bool Reader::read_all(String& string) const
	{
		if (!_private)
			return false;
		if (_private->_size > std::numeric_limits<size_t>::max() - 1) // One extra byte for zero terminator.
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

	Buffer Reader::to_buffer() const
	{
		Buffer buffer;
		return read_all(buffer) ? std::move(buffer) : Buffer();
	}

	String Reader::to_string(Allocator& allocator) const
	{
		String string(&allocator);
		return read_all(string) ? std::move(string) : String(&allocator);
	}

	Reader::Reader() noexcept = default;
	Reader::Reader(Reader&&) noexcept = default;
	Reader::~Reader() = default;
	Reader& Reader::operator=(Reader&&) noexcept = default;
}
