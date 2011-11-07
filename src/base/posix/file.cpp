#define _LARGEFILE64_SOURCE
#define _XOPEN_SOURCE 500

#define __USE_LARGEFILE64 // NOTE: Deprecated?

#include "file.hpp"

#include <fcntl.h>  // open
#include <unistd.h> // close, dup, fsync, ftruncate64, lseek64, pread64, pwrite64

namespace Yttrium
{

FileReaderImpl::FileReaderImpl() throw()
	: FileReader(0)
	, _descriptor(-1)
	, _offset(0)
	, _size(0)
{
}

bool FileReaderImpl::open_file(const StaticString &name) throw()
{
	if (_descriptor != -1)
	{
		return false;
	}

	_descriptor = ::open(name.text(), O_RDONLY | O_LARGEFILE, 0644);
	if (_descriptor == -1)
	{
		return false;
	}

	_size = lseek64(_descriptor, 0, SEEK_END);
	return true;
}

FileReaderImpl::~FileReaderImpl() throw()
{
	close(_descriptor);
}

FileReaderPtr FileReaderImpl::dup(Allocator *allocator)
{
	int descriptor = ::dup(_descriptor);
	if (descriptor == -1)
	{
		return FileReaderPtr();
	}

	try
	{
		return FileReaderPtr(new(allocator) FileReaderImpl(descriptor, _offset, _size));
	}
	catch (...)
	{
		close(descriptor);
		throw;
	}
}

size_t FileReaderImpl::read(void *buffer, size_t size) throw()
{
	ssize_t result = pread64(_descriptor, buffer, size, _offset);
	if (result != -1)
	{
		_offset += static_cast<size_t>(result);
		return result;
	}
	return 0;
}

UOffset FileReaderImpl::offset() throw()
{
	return _offset;
}

bool FileReaderImpl::seek(Offset offset, Whence whence) throw()
{
	UOffset newOffset;

	switch (whence)
	{
	case Relative: newOffset = offset + _offset; break;
	case Reverse:  newOffset = offset + _size;   break;
	default:       newOffset = offset;           break;
	}
	if (newOffset > _size)
	{
		return false;
	}
	_offset = newOffset;
	return true;
}

UOffset FileReaderImpl::size() throw()
{
	return _size;
}

FileReaderImpl::FileReaderImpl(int descriptor) throw()
	: _descriptor(descriptor)
	, _offset(0)
	, _size(lseek64(descriptor, 0, SEEK_END))
{
}

FileReaderImpl::FileReaderImpl(int descriptor, UOffset offset, UOffset size) throw()
	: _descriptor(descriptor)
	, _offset(offset)
	, _size(size)
{
}

FileReaderPtr FileReader::open(const StaticString &name, Allocator *allocator)
{
	int descriptor = ::open(name.text(), O_RDONLY | O_LARGEFILE, 0644);
	if (descriptor == -1)
	{
		return FileReaderPtr();
	}

	try
	{
		return FileReaderPtr(new(allocator) FileReaderImpl(descriptor));
	}
	catch (...)
	{
		close(descriptor);
		throw;
	}
}

FileWriterImpl::FileWriterImpl() throw()
	: FileWriter(0)
	, _descriptor(-1)
	, _offset(0)
{
}

bool FileWriterImpl::open_file(const StaticString &name) throw()
{
	if (_descriptor != -1)
	{
		return false;
	}

	_descriptor = ::open(name.text(), O_WRONLY | O_CREAT | O_LARGEFILE, 0644);
	if (_descriptor == -1)
	{
		return false;
	}

	return true;
}

FileWriterImpl::~FileWriterImpl() throw()
{
	close(_descriptor);
}

FileWriterPtr FileWriterImpl::dup(Allocator *allocator)
{
	int descriptor = ::dup(_descriptor);
	if (descriptor == -1)
	{
		return FileWriterPtr();
	}

	try
	{
		return FileWriterPtr(new(allocator) FileWriterImpl(descriptor, _offset));
	}
	catch (...)
	{
		close(descriptor);
		throw;
	}
}

size_t FileWriterImpl::write(const void *buffer, size_t size) throw()
{
	ssize_t result = pwrite64(_descriptor, buffer, size, _offset);
	if (result != -1)
	{
		_offset += static_cast<size_t>(result);
		return result;
	}
	return 0;
}

bool FileWriterImpl::flush() throw()
{
	return !fsync(_descriptor);
}

UOffset FileWriterImpl::offset() throw()
{
	return _offset;
}

bool FileWriterImpl::seek(Offset offset, Whence whence) throw()
{
	switch (whence)
	{
	case Relative: _offset = offset + _offset; break;
	case Reverse:  _offset = offset + size();  break;
	default:       _offset = offset;           break;
	}
	return true;
}

UOffset FileWriterImpl::size() throw()
{
	off64_t result = lseek64(_descriptor, 0, SEEK_END);
	return result != -1 ? result : 0;
}

bool FileWriterImpl::resize(UOffset size) throw()
{
	return !ftruncate64(_descriptor, size);
}

bool FileWriterImpl::truncate() throw()
{
	return !ftruncate64(_descriptor, _offset);
}

FileWriterImpl::FileWriterImpl(int descriptor) throw()
	: _descriptor(descriptor)
	, _offset(0)
{
}

FileWriterImpl::FileWriterImpl(int descriptor, UOffset offset) throw()
	: _descriptor(descriptor)
	, _offset(offset)
{
}

FileWriterPtr FileWriter::open(const StaticString &name, Allocator *allocator)
{
	int descriptor = ::open(name.text(), O_WRONLY | O_CREAT | O_LARGEFILE, 0644);
	if (descriptor == -1)
	{
		return FileWriterPtr();
	}

	try
	{
		return FileWriterPtr(new(allocator) FileWriterImpl(descriptor));
	}
	catch (...)
	{
		close(descriptor);
		throw;
	}
}

} // namespace Yttrium
