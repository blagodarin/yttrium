#define _FILE_OFFSET_BITS 64

#include "../../utils/zero_terminated.h"
#include "file.h"

#include <system_error>

#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

namespace Yttrium
{
	File::Private::Private(String&& name, int descriptor, unsigned mode, Allocator* allocator)
		: PrivateBase(allocator)
		, _descriptor(descriptor)
		, _mode(mode)
		, _name(name)
		, _auto_close(true)
		, _auto_remove(false)
	{
		assert(_descriptor != -1);
		assert(_mode & ReadWrite);
	}

	File::Private::~Private()
	{
		if (_auto_close)
		{
			::close(_descriptor);
			if (_auto_remove && ::unlink(_name.text()))
				throw std::system_error(errno, std::generic_category());
		}
	}

	int File::Private::open(const StaticString& name, int flags)
	{
		Y_ZERO_TERMINATED(name_z, name);
		return ::open(name_z, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	}

	File::File(const StaticString& name, unsigned mode, Allocator* allocator)
		: File()
	{
		int flags = Y_PLATFORM_LINUX ? O_NOATIME : 0;
		switch (mode & ReadWrite)
		{
		case Read:      flags |= O_RDONLY;          break;
		case Write:     flags = O_WRONLY | O_CREAT; break;
		case ReadWrite: flags |= O_RDWR | O_CREAT;  break;
		default:        return;
		}

		if ((mode & (Write | Pipe | Truncate)) == (Write | Truncate))
			flags |= O_TRUNC;

		const int descriptor = Private::open(name, flags);
		if (descriptor == -1)
			return;

		_private = Y_NEW(allocator, Private)(String(name, allocator), descriptor, mode, allocator);
		if ((mode & (Read | Write | Pipe)) == Read)
			_size = ::lseek(descriptor, 0, SEEK_END);
	}

	File::File(Special special, Allocator* allocator)
		: File()
	{
		switch (special)
		{
		case Temporary:
			{
				String name("/tmp/XXXXXX"_s, allocator);

				const auto descriptor = ::mkstemp(name.text());
				if (descriptor == -1)
					break;

				_private = Y_NEW(allocator, Private)(std::move(name), descriptor, ReadWrite, allocator);
				_private->_auto_remove = true;
			}
			break;

		case StdErr:
			_private = Y_NEW(allocator, Private)(String(allocator), STDERR_FILENO, Write | Pipe, allocator);
			_private->_auto_close = false;
			break;
		}
	}

	bool File::flush()
	{
		return _private && (_private->_mode & Write)
			? !::fsync(_private->_descriptor)
			: false;
	}

	size_t File::read(void* buffer, size_t size)
	{
		if (_private && (_private->_mode & Read))
		{
			ssize_t result;

			if (_private->_mode & Pipe)
			{
				result = ::read(_private->_descriptor, buffer, size);
				if (result != -1)
					return result;
			}
			else
			{
				result = ::pread(_private->_descriptor, buffer, size, _base + _offset);
				if (result != -1)
				{
					_offset += static_cast<size_t>(result);
					return result;
				}
			}
		}
		return 0;
	}

	bool File::resize(uint64_t size)
	{
		return (_private && ((_private->_mode & (Write | Pipe)) == Write)
			? !::ftruncate(_private->_descriptor, _base + size)
			: false);
	}

	bool File::seek(uint64_t offset, Whence whence)
	{
		if (!_private || (_private->_mode & Pipe))
			return false;

		bool read_only = ((_private->_mode & ReadWrite) == Read);

		switch (whence)
		{
		case Relative:

			{
				uint64_t limit = (read_only ? _size : UINT64_MAX);
				if (limit - _offset < offset)
					return false;
				_offset += offset;
			}
			break;

		case Reverse:

			{
				uint64_t size;
				if (read_only)
				{
					size = _size;
				}
				else
				{
					off_t off = ::lseek(_private->_descriptor, 0, SEEK_END);
					if (off == -1)
						throw std::system_error(errno, std::generic_category());
					size = static_cast<uint64_t>(off);
				}
				if (size < offset)
					return false;
				_offset = size - offset;
			}
			break;

		default:

			if (read_only && offset > _size)
				return false;
			_offset = offset;
			break;
		}
		return true;
	}

	uint64_t File::size() const
	{
		if (!_private)
			return 0;

		switch (_private->_mode & (ReadWrite | Pipe))
		{
		case Read:
			return _size;

		case Write:
		case ReadWrite:
			{
				off_t size = ::lseek(_private->_descriptor, 0, SEEK_END);
				if (size == -1)
					throw std::system_error(errno, std::generic_category());
				return size - _base;
			}

		default:
			return 0;
		}
	}

	size_t File::write(const void* buffer, size_t size)
	{
		if (_private && (_private->_mode & Write))
		{
			if (_private->_mode & Pipe)
			{
				const ssize_t result = ::write(_private->_descriptor, buffer, size);
				if (result != -1)
					return result;
			}
			else
			{
				const ssize_t result = ::pwrite(_private->_descriptor, buffer, size, _base + _offset);
				if (result != -1)
				{
					_offset += static_cast<size_t>(result);
					return result;
				}
			}
		}
		return 0;
	}
}
