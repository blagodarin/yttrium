#include "reader.h"

#include <yttrium/memory/buffer.h>
#include <yttrium/static_string.h>
#include <yttrium/storage/temporary_file.h>
#include <yttrium/utils.h>
#include "../system/file.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>

namespace Yttrium
{
	BufferReader::BufferReader(Buffer&& buffer)
		: ReaderPrivate(buffer.size())
		, _buffer(std::make_shared<const Buffer>(std::move(buffer)))
	{
	}

	BufferReader::BufferReader(const std::shared_ptr<const Buffer>& buffer, const std::string& name)
		: ReaderPrivate(buffer->size(), name)
		, _buffer(buffer)
	{
	}

	size_t BufferReader::read_at(uint64_t offset, void* data, size_t size) const
	{
		std::memcpy(data, static_cast<const uint8_t*>(_buffer->data()) + offset, size);
		return size;
	}

	const void* BufferReader::data() const noexcept
	{
		return _buffer->data();
	}

	ReaderReader::ReaderReader(const std::shared_ptr<const ReaderPrivate>& reader, uint64_t base, uint64_t size)
		: ReaderPrivate(size)
		, _reader(reader)
		, _base(base)
	{
	}

	size_t ReaderReader::read_at(uint64_t offset, void* data, size_t size) const
	{
		return offset <= std::numeric_limits<uint64_t>::max() - _base ? _reader->read_at(_base + offset, data, size) : 0;
	}

	SpanReader::SpanReader(const void* data, size_t size)
		: ReaderPrivate(size)
		, _data(data)
	{
	}

	size_t SpanReader::read_at(uint64_t offset, void* data, size_t size) const
	{
		std::memcpy(data, static_cast<const uint8_t*>(_data) + offset, size);
		return size;
	}

	const void* SpanReader::data() const noexcept
	{
		return _data;
	}

	Reader::Reader(const void* data, size_t size)
		: _private(std::make_shared<SpanReader>(data, size))
	{
	}

	Reader::Reader(Buffer&& buffer)
		: _private(std::make_shared<BufferReader>(std::move(buffer)))
	{
	}

	Reader::Reader(const std::string& path)
		: _private(create_file_reader(path))
	{
	}

	Reader::Reader(const TemporaryFile& file)
		: _private(create_file_reader(file))
	{
	}

	Reader::Reader(const Reader& reader, uint64_t base, uint64_t size)
		: _private(base < reader.size() ? std::make_shared<ReaderReader>(reader._private, base, min(size, reader.size() - base)) : nullptr)
	{
	}

	StaticString Reader::name() const
	{
		return _private ? StaticString{ _private->_name } : StaticString{};
	}

	uint64_t Reader::offset() const
	{
		return _private ? _private->_offset : 0;
	}

	StaticString Reader::property(const StaticString& name) const
	{
		return _private ? _private->_properties.find(name) : StaticString();
	}

	size_t Reader::read(void* data, size_t size)
	{
		if (!size || !_private || _private->_offset == _private->_size)
			return 0;
		const auto result = _private->read_at(_private->_offset, data, min<uint64_t>(size, _private->_size - _private->_offset));
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

	bool Reader::read_all(std::string& string) const
	{
		if (!_private)
			return false;
		if (_private->_size > std::numeric_limits<size_t>::max() - 1) // One extra byte for null terminator.
			throw std::bad_alloc();
		string.resize(_private->_size);
		return _private->read_at(0, const_cast<char*>(string.data()), string.size()) == string.size();
	}

	size_t Reader::read_at(uint64_t offset, void* data, size_t size) const
	{
		if (!size || !_private || offset >= _private->_size)
			return 0;
		return _private->read_at(offset, data, min<uint64_t>(size, _private->_size - offset));
	}

	bool Reader::read_line(std::string& string)
	{
		if (!_private)
			return false;
		constexpr size_t buffer_size = 32;
		std::array<char, buffer_size + 1> buffer;
		string.clear();
		for (;;)
		{
			// TODO: Copy string data if reading from memory.
			const auto bytes_read = read_at(_private->_offset + string.size(), buffer.data(), buffer_size);
			const auto data_end = buffer.begin() + bytes_read;
			const auto text_end = std::find_if(buffer.begin(), data_end, [](char c){ return c == '\r' || c == '\n'; });
			string.append(buffer.begin(), text_end);
			if (text_end < data_end)
			{
				_private->_offset += string.size() + (*text_end == '\r'
					&& (text_end + 1 < data_end || read_at(_private->_offset + string.size() + 1, &buffer[buffer_size], 1))
					&& *(text_end + 1) == '\n' ? 2 : 1);
				return true;
			}
			if (bytes_read < buffer.size())
			{
				_private->_offset += string.size();
				return !string.empty();
			}
		}
	}

	bool Reader::seek(uint64_t offset)
	{
		if (!_private || offset > _private->_size)
			return false;
		_private->_offset = offset;
		return true;
	}

	void Reader::set_property(const StaticString& name, const StaticString& value)
	{
		if (_private)
			_private->_properties.insert_or_assign(name, value);
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

	std::string Reader::to_string() const
	{
		std::string string;
		return read_all(string) ? string : std::string{};
	}

	Reader::Reader() noexcept = default;
	Reader::Reader(Reader&&) noexcept = default;
	Reader::Reader(std::shared_ptr<class ReaderPrivate>&& private_) noexcept : _private(std::move(private_)) {}
	Reader::~Reader() = default;
	Reader& Reader::operator=(Reader&&) noexcept = default;
}
