#define _FILE_OFFSET_BITS 64
//#define _XOPEN_SOURCE 500

#include "file.hpp"

#include <fcntl.h>  // open
#include <unistd.h> // close, fsync, ftruncate, lseek, pread, pwrite, read, write

#include <Yttrium/string.hpp>

namespace Yttrium
{

File::File(const StaticString &name, Mode mode, Allocator *allocator) noexcept
	: _private(nullptr)
	, _offset(0)
	, _size(0)
	, _base(0)
{
	open(name, mode, allocator);
}

File::File(const File &file) noexcept
	: _private(file._private)
	, _offset(file._offset)
	, _size(file._size)
	, _base(file._base)
{
	// NOTE: It may be theoretically possible that our copy-constructing thread will be interrupted
	// just here, and then the file would be destroyed in another thread, leaving us with a crash.

	if (_private)
	{
		++_private->references;
	}
}

File::~File() noexcept
{
	close();
}

void File::close() noexcept
{
	if (_private)
	{
		if (_private->allocator)
		{
			if (!--_private->references)
			{
				::close(_private->descriptor);
				_private->allocator->delete_(_private);
			}
			_private = nullptr;
		}
		else if (_private->mode & ReadWrite)
		{
			::close(_private->descriptor);
			_private->mode = 0;
		}
	}
	_offset = 0;
	_size = 0;
	_base = 0;
}

bool File::flush() noexcept
{
	return (_private && (_private->mode & Write)
		? !fsync(_private->descriptor)
		: false);
}

bool File::open(const StaticString &name, Mode mode, Allocator *allocator) noexcept
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
		_private = new(allocator->allocate<Private>())
			Private(descriptor, mode, allocator);

		if ((_private->mode & (Read | Write | Pipe)) == Read)
		{
			_size = lseek(descriptor, 0, SEEK_END);
		}
	}
	else
	{
		_private->descriptor = descriptor;
		_private->mode = mode;
	}

	return true;
}

bool File::opened() const noexcept
{
	return (_private && (_private->mode & ReadWrite));
}

size_t File::read(void *buffer, size_t size) noexcept
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

bool File::resize(UOffset size) noexcept
{
	return (_private && ((_private->mode & (Write | Pipe)) == Write)
		? !ftruncate(_private->descriptor, size)
		: false);
}

bool File::seek(UOffset offset, Whence whence) noexcept
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
					return false;
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

UOffset File::size() const noexcept
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
				if (size != -1)
				{
					return size;
				}
			}
			break;
		}
	}
	return 0;
}

bool File::truncate() noexcept
{
	return resize(_offset);
}

size_t File::write(const void *buffer, size_t size) noexcept
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
			ssize_t result = pwrite(_private->descriptor, buffer, size, _offset);
			if (result != -1)
			{
				_offset += static_cast<size_t>(result);
				return result;
			}
		}
	}
	return 0;
}

int File::Private::open(const StaticString &name, int flags, Allocator *allocator) noexcept
{
	if (name.zero_terminated()) // Avoid extra allocation.
	{
		return ::open(name.text(), flags, 0644);
	}
	else
	{
		String fixed_name(name, allocator); // TODO: Think of using a stack (alloca) here.

		return ::open(fixed_name.text(), flags, 0644);
	}
}

} // namespace Yttrium
