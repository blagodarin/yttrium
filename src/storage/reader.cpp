#include "reader.h"

#include <yttrium/memory/buffer.h>
#include <yttrium/string.h>
#include <yttrium/utils.h>
#include "../system/file.h"

#include <cstring>
#include <limits>

namespace Yttrium
{
	BufferReader::BufferReader(Buffer&& buffer)
		: ReaderPrivate(buffer.size())
		, _buffer(std::make_shared<const Buffer>(std::move(buffer)))
	{
	}

	BufferReader::BufferReader(const std::shared_ptr<const Buffer>& buffer, const String& name)
		: ReaderPrivate(buffer->size(), name)
		, _buffer(buffer)
	{
	}

	size_t BufferReader::read_at(uint64_t offset, void* data, size_t size) const
	{
		std::memcpy(data, static_cast<const uint8_t*>(_buffer->data()) + offset, size);
		return size;
	}

	ReaderReader::ReaderReader(const std::shared_ptr<const ReaderPrivate>& reader, uint64_t base, uint64_t size)
		: ReaderPrivate(size)
		, _reader(reader)
		, _base(base)
	{
	}

	ReaderReader::ReaderReader(const std::shared_ptr<const ReaderPrivate>& reader, uint64_t base, uint64_t size, const String& name)
		: ReaderPrivate(size, name)
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

	Reader::Reader(const void* data, size_t size)
		: _private(std::make_shared<SpanReader>(data, size))
	{
	}

	Reader::Reader(Buffer&& buffer)
		: _private(std::make_shared<BufferReader>(std::move(buffer)))
	{
	}

	Reader::Reader(const StaticString& path)
		: _private(create_file_reader(path))
	{
	}

	Reader::Reader(const Reader& reader, uint64_t base, uint64_t size)
		: _private(base < reader.size() ? std::make_shared<ReaderReader>(reader._private, base, min(size, reader.size() - base)) : nullptr)
	{
	}

	StaticString Reader::name() const
	{
		return _private ? StaticString(_private->_name) : StaticString();
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

	bool Reader::read_all(String& string) const
	{
		if (!_private)
			return false;
		if (_private->_size > std::numeric_limits<size_t>::max() - 1) // One extra byte for zero terminator.
			throw std::bad_alloc();
		string.resize(_private->_size);
		return _private->read_at(0, string.text(), string.size()) == string.size();
	}

	size_t Reader::read_at(uint64_t offset, void* data, size_t size) const
	{
		if (!size || !_private || offset >= _private->_size)
			return 0;
		return _private->read_at(offset, data, min<uint64_t>(size, _private->_size - offset));
	}

	bool Reader::read_line(String& string)
	{
		if (!_private)
			return false;

		static const size_t buffer_step = 32;

		const auto origin = _private->_offset;

		string.clear();

		for (size_t offset = 0, bytes_read = 0; ; offset += bytes_read)
		{
			string.resize(offset + buffer_step);

			bytes_read = read(string.text() + offset, buffer_step);

			string.resize(offset + bytes_read);

			if (!bytes_read)
			{
				seek(origin + string.size());
				return offset; // Return 'false' if we haven't read anything.
			}

			const auto r_offset = string.find_first('\r', offset);

			if (r_offset != String::End)
			{
				size_t string_size = string.size();

				if (r_offset == string_size - 1)
				{
					string.resize(string_size + 1);
					bytes_read += read(string.text() + string_size, 1);
				}

				string.resize(r_offset);
				seek(origin + r_offset + 1 + (string[r_offset + 1] == '\n'));
				break;
			}
			else
			{
				const auto n_offset = string.find_first('\n', offset);
				if (n_offset != String::End)
				{
					string.resize(n_offset);
					seek(origin + n_offset + 1);
					break;
				}
			}
		}

		return true;
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

	String Reader::to_string(Allocator& allocator) const
	{
		String string(&allocator);
		return read_all(string) ? std::move(string) : String(&allocator);
	}

	Reader::Reader() noexcept = default;
	Reader::Reader(Reader&&) noexcept = default;
	Reader::Reader(std::shared_ptr<class ReaderPrivate>&& private_) noexcept : _private(std::move(private_)) {}
	Reader::~Reader() = default;
	Reader& Reader::operator=(Reader&&) noexcept = default;
}
