#include "file.h"

#include <yttrium/memory/buffer.h>
#include <yttrium/utils.h>

#include <cassert>
#include <new>

namespace Yttrium
{
	FilePrivate::FilePrivate(String&& name, unsigned mode, uint64_t size)
		: _name(std::move(name))
		, _mode(mode)
		, _size(size)
	{
		assert(_mode & File::ReadWrite);
	}

	File::File(const StaticString& path, unsigned mode, Allocator& allocator)
		: _private(FilePrivate::open(path, mode, allocator))
	{
	}

	File::File(Special special, Allocator& allocator)
		: _private(FilePrivate::open(special, allocator))
	{
	}

	bool File::flush()
	{
		return _private && _private->flush();
	}

	StaticString File::name() const
	{
		return _private ? StaticString(_private->_name) : StaticString();
	}

	uint64_t File::offset() const
	{
		return _private ? _private->_offset : 0;
	}

	size_t File::read(void* buffer, size_t size)
	{
		if (!size || !_private || !(_private->_mode & Read))
			return 0;
		if (_private->_mode & Pipe)
			return _private->read(buffer, size);
		if (_private->_offset == _private->_size)
			return 0;
		const auto result = _private->read(buffer, min<uint64_t>(size, _private->_size - _private->_offset), _private->_offset);
		_private->_offset += result;
		return result;
	}

	bool File::read_line(String& string)
	{
		if (!_private || (_private->_mode & (Read | Pipe)) != Read) // TODO: Make it work for pipes too.
			return false;

		static const size_t buffer_step = 32;

		uint64_t file_offset = _private->_offset;

		string.clear();

		for (size_t offset = 0, bytes_read; ; offset += bytes_read)
		{
			string.resize(offset + buffer_step);

			bytes_read = read(string.text() + offset, buffer_step);

			string.resize(offset + bytes_read);

			if (!bytes_read)
			{
				seek(file_offset + string.size());
				return offset; // Return 'false' if we haven't read anything.
			}

			size_t r_offset = string.find_first('\r', offset);

			if (r_offset != String::End)
			{
				size_t string_size = string.size();

				if (r_offset == string_size - 1)
				{
					string.resize(string_size + 1);
					bytes_read += read(string.text() + string_size, 1);
				}

				string.resize(r_offset);
				seek(file_offset + r_offset + 1 + (string[r_offset + 1] == '\n'));
				break;
			}
			else
			{
				size_t n_offset = string.find_first('\n', offset);

				if (n_offset != String::End)
				{
					string.resize(n_offset);
					seek(file_offset + n_offset + 1);
					break;
				}
			}
		}

		return true;
	}

	bool File::resize(uint64_t size)
	{
		if (!_private || (_private->_mode & (Write | Pipe)) != Write || !_private->resize(size))
			return false;
		_private->_size = size;
		return true;
	}

	bool File::seek(uint64_t offset, Whence whence)
	{
		if (_private && !(_private->_mode & Pipe))
		{
			switch (whence)
			{
			case Absolute:
				if (offset <= _private->_size)
				{
					_private->_offset = offset;
					return true;
				}
				break;
			case Relative:
				if (offset <= _private->_size - _private->_offset)
				{
					_private->_offset += offset;
					return true;
				}
				break;
			case Reverse:
				if (offset <= _private->_size)
				{
					_private->_offset = _private->_size - offset;
					return true;
				}
				break;
			}
		}
		return false;
	}

	uint64_t File::size() const
	{
		return _private ? _private->_size : 0;
	}

	size_t File::write(const void* buffer, size_t size)
	{
		if (!_private || !(_private->_mode & Write))
			return 0;
		if (_private->_mode & Pipe)
			return _private->write(buffer, size);
		const auto result = _private->write(buffer, size, _private->_offset);
		_private->_offset += result;
		if (_private->_size < _private->_offset)
			_private->_size = _private->_offset;
		return result;
	}

	File::File() noexcept = default;
	File::File(File&&) noexcept = default;
	File::~File() = default;
	File& File::operator=(File&&) noexcept = default;
}
