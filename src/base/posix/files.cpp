#define _LARGEFILE64_SOURCE
#define _XOPEN_SOURCE 500

#define __USE_LARGEFILE64 // NOTE: Deprecated?

#include <fcntl.h>  // open
#include <unistd.h> // close, dup, fsync, ftruncate64, lseek64, pread64, pwrite64

#include <Yttrium/files.hpp>

namespace Yttrium
{

class FileReaderImpl: public FileReader
{
public:

	FileReaderImpl(int descriptor) throw()
		: _descriptor(descriptor)
		, _offset(0)
		, _size(lseek64(descriptor, 0, SEEK_END))
	{
	}

	FileReaderImpl(int descriptor, UOffset offset, UOffset size) throw()
		: _descriptor(descriptor)
		, _offset(offset)
		, _size(size)
	{
	}

	virtual ~FileReaderImpl() throw()
	{
		close(_descriptor);
	}

	virtual FileReaderPtr dup(Allocator *allocator)
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

	virtual size_t read(void *buffer, size_t size) throw()
	{
		ssize_t result = pread64(_descriptor, buffer, size, _offset);
		if (result != -1)
		{
			_offset += static_cast<size_t>(result);
			return result;
		}
		return 0;
	}

	virtual UOffset offset() throw()
	{
		return _offset;
	}

	virtual bool seek(Offset offset, Whence whence) throw()
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

	virtual UOffset size() throw()
	{
		return _size;
	}

private:

	int     _descriptor;
	UOffset _offset;
	UOffset _size;
};

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

class FileWriterImpl: public FileWriter
{
public:

	FileWriterImpl(int descriptor) throw()
		: _descriptor(descriptor)
		, _offset(0)
	{
	}

	FileWriterImpl(int descriptor, UOffset offset) throw()
		: _descriptor(descriptor)
		, _offset(offset)
	{
	}

	virtual ~FileWriterImpl() throw()
	{
		close(_descriptor);
	}

	virtual FileWriterPtr dup(Allocator *allocator)
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

	size_t write(const void *buffer, size_t size) throw()
	{
		ssize_t result = pwrite64(_descriptor, buffer, size, _offset);
		if (result != -1)
		{
			_offset += static_cast<size_t>(result);
			return result;
		}
		return 0;
	}

	virtual bool flush() throw()
	{
		return !fsync(_descriptor);
	}

	virtual UOffset offset() throw()
	{
		return _offset;
	}

	virtual bool seek(Offset offset, Whence whence) throw()
	{
		switch (whence)
		{
		case Relative: _offset = offset + _offset; break;
		case Reverse:  _offset = offset + size();  break;
		default:       _offset = offset;           break;
		}
		return true;
	}

	virtual UOffset size() throw()
	{
		off64_t result = lseek64(_descriptor, 0, SEEK_END);
		return result != -1 ? result : 0;
	}

	virtual bool resize(UOffset size) throw()
	{
		return !ftruncate64(_descriptor, size);
	}

	virtual bool truncate() throw()
	{
		return !ftruncate64(_descriptor, _offset);
	}

private:

	int     _descriptor;
	UOffset _offset;
};

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
