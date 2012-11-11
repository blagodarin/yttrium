#include "file.h"

#include <Yttrium/assert.h>
#include <Yttrium/string.h>

#define _FILE_OFFSET_BITS 64

#include <cstdlib>  // mkstemp
#include <fcntl.h>  // open
#include <unistd.h> // close, fsync, ftruncate, lseek, pread, pwrite, read, write

namespace Yttrium
{

File::Private::~Private()
{
	::close(descriptor);

	if (auto_remove)
	{
		::unlink(name.const_text());
	}
}

bool File::flush()
{
	return (_private && (_private->mode & Write)
		? !fsync(_private->descriptor)
		: false);
}

bool File::open(const StaticString &name, Mode mode, Allocator *allocator)
{
	close();

	int flags = (Y_IS_LINUX ? O_NOATIME : 0);
	switch (mode & ReadWrite)
	{
	case Read:      flags |= O_RDONLY;           break;
	case Write:     flags |= O_WRONLY | O_CREAT; break;
	case ReadWrite: flags = O_RDWR | O_CREAT;    break;
	default:        return false;
	}

	int descriptor = Private::open(name, flags, allocator);

	if (descriptor == -1)
	{
		return false;
	}

	if (!_private)
	{
		_private = Y_NEW(allocator, Private)(allocator);
	}

	_private->descriptor = descriptor;
	_private->mode = mode;

	if ((_private->mode & (Read | Write | Pipe)) == Read)
	{
		_size = lseek(descriptor, 0, SEEK_END);
	}

	return true;
}

bool File::open(Special special, Allocator *allocator)
{
	close();

	switch (special)
	{
	case Temporary:

		{
			String name(S("/tmp/XXXXXX"), allocator);

			int descriptor = ::mkstemp(name.text());

			if (descriptor == -1)
			{
				break;
			}

			if (!_private)
			{
				_private = Y_NEW(allocator, Private)(allocator);
			}

			_private->descriptor = descriptor;
			_private->mode = Read | Write;
			_private->name = name;
			_private->auto_remove = true;

			return true;
		}
		break;
	}

	return false;
}

size_t File::read(void *buffer, size_t size)
{
	if (_private && (_private->mode & Read))
	{
		ssize_t result;

		if (_private->mode & Pipe)
		{
			result = ::read(_private->descriptor, buffer, size);
			if (result != -1)
			{
				return result;
			}
		}
		else
		{
			result = pread(_private->descriptor, buffer, size, _base + _offset);
			if (result != -1)
			{
				_offset += static_cast<size_t>(result);
				return result;
			}
		}
	}
	return 0;
}

bool File::resize(UOffset size)
{
	return (_private && ((_private->mode & (Write | Pipe)) == Write)
		? !ftruncate(_private->descriptor, _base + size)
		: false);
}

bool File::seek(UOffset offset, Whence whence)
{
	if (!_private || (_private->mode & Pipe))
	{
		return false;
	}

	bool read_only = ((_private->mode & ReadWrite) == Read);

	switch (whence)
	{
	case Relative:

		{
			UOffset limit = (read_only ? _size : UINT64_MAX); // NOTE: Hack: we don't exactly know the UOffset's size.

			if (limit - _offset < offset)
			{
				return false;
			}
			_offset += offset;
		}
		break;

	case Reverse:

		{
			UOffset size;

			if (read_only)
			{
				size = _size;
			}
			else
			{
				off_t off = lseek(_private->descriptor, 0, SEEK_END);

				if (off == -1)
				{
					return false; // TODO: Y_ABORT.
				}
				size = static_cast<UOffset>(off);
			}
			if (size < offset)
			{
				return false;
			}
			_offset = size - offset;
		}
		break;

	default:

		if (read_only && offset > _size)
		{
			return false;
		}
		_offset = offset;
		break;
	}
	return true;
}

UOffset File::size() const
{
	if (_private)
	{
		switch (_private->mode & (ReadWrite | Pipe))
		{
		case Read:

			return _size;

		case Write:
		case ReadWrite:

			{
				off_t size = lseek(_private->descriptor, 0, SEEK_END);
				if (size == -1)
				{
					break; // TODO: Y_ABORT.
				}
				return size - _base;
			}
		}
	}

	return 0;
}

size_t File::write(const void *buffer, size_t size)
{
	if (_private && (_private->mode & Write))
	{
		if (_private->mode & Pipe)
		{
			ssize_t result = ::write(_private->descriptor, buffer, size);
			if (result != -1)
			{
				return result;
			}
		}
		else
		{
			ssize_t result = pwrite(_private->descriptor, buffer, size, _base + _offset);
			if (result != -1)
			{
				_offset += static_cast<size_t>(result);
				return result;
			}
		}
	}
	return 0;
}

int File::Private::open(const StaticString &name, int flags, Allocator *allocator)
{
	return ::open(name.zero_terminated(allocator).text(), flags, 0644); // TODO: Think of using a stack (alloca) here.
}

} // namespace Yttrium
