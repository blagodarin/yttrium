#include <yttrium/io/file.h>

#include <yttrium/static_string.h>
#include <yttrium/utils.h>
#include "file.h"

namespace Yttrium
{
	File::File(const StaticString& path, unsigned mode)
		: _private(FilePrivate::open(path, mode))
	{
	}

	File::File(Special special)
		: _private(FilePrivate::open(special))
	{
	}

	bool File::flush()
	{
		return _private && _private->flush();
	}

	StaticString File::name() const
	{
		return _private ? StaticString(_private->_name.data(), _private->_name.size()) : StaticString();
	}

	uint64_t File::offset() const
	{
		return _private ? _private->_offset : 0;
	}

	size_t File::read(void* data, size_t size)
	{
		if (!size || !_private || !(_private->_mode & Read))
			return 0;
		if (_private->_mode & Pipe)
			return _private->read(data, size);
		if (_private->_offset == _private->_size)
			return 0;
		const auto result = _private->read_at(_private->_offset, data, min<uint64_t>(size, _private->_size - _private->_offset));
		_private->_offset += result;
		return result;
	}

	bool File::resize(uint64_t size)
	{
		if (!_private || (_private->_mode & (Write | Pipe)) != Write || !_private->resize(size))
			return false;
		_private->_size = size;
		_private->_offset = min(_private->_offset, _private->_size);
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

	size_t File::write(const void* data, size_t size)
	{
		if (!_private || !(_private->_mode & Write))
			return 0;
		if (_private->_mode & Pipe)
			return _private->write(data, size);
		const auto result = _private->write_at(_private->_offset, data, size);
		_private->_offset += result;
		if (_private->_size < _private->_offset)
			_private->_size = _private->_offset;
		return result;
	}

	size_t File::write_at(uint64_t offset, const void* data, size_t size)
	{
		if (!_private || !(_private->_mode & Write) || _private->_mode & Pipe || offset > _private->_size)
			return 0;
		const auto result = _private->write_at(offset, data, size);
		_private->_size = max(_private->_size, offset + result);
		return result;
	}

	File::File() noexcept = default;
	File::File(File&&) noexcept = default;
	File::~File() = default;
	File& File::operator=(File&&) noexcept = default;
}
