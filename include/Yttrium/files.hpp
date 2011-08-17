/// \file
/// \brief File facilities.

#ifndef __Y_FILES_HPP
#define __Y_FILES_HPP

#include <Yttrium/allocators.hpp>
#include <Yttrium/object.hpp>
#include <Yttrium/static_string.hpp>
#include <Yttrium/types.hpp>

namespace Yttrium
{

/// Valid "whences" (origins) for the the seek functions.

enum Whence
{
	Absolute, ///< Offset from the beginning of file.
	Relative, ///< Offset from the current position.
	Reverse,  ///< Offset from the end of file (added).
};

class FileReader;

///

typedef ObjectPtr<FileReader> FileReaderPtr;

/// File reader.

class Y_API FileReader: public Object
{
public:

	/// Create a duplicate FileReader.
	/// \return Duplicate FileReader or NULL.

	virtual FileReaderPtr dup(Allocator *allocator = HeapAllocator::instance()) = 0;

	/// Read the specified amount of bytes into the buffer.
	/// \return Number of bytes read or 0 on failure.

	virtual size_t read(void *buffer, size_t size) throw() = 0;

	/// Read the buffer from the file.
	/// \param buffer Buffer to read.
	/// \return \c true on success.

	template <typename T>
	bool read(T &buffer) throw()
	{
		return read(&buffer, sizeof buffer) == sizeof buffer;
	}

	/// Return the current file offset.
	/// \return Current offset.

	virtual UOffset offset() throw() = 0;

	/// Set the file offset to the specified position.

	virtual bool seek(Offset offset, Whence whence = Absolute) throw() = 0;

	/// Return the file size.
	/// \return File size.

	virtual UOffset size() throw() = 0;

public:

	/// Create a file reader.

	static FileReaderPtr open(const StaticString &name, Allocator *allocator = HeapAllocator::instance());

protected:

	FileReader(size_t counter = 1) throw()
		: Object(counter)
	{
	}
};

class FileWriter;

///

typedef ObjectPtr<FileWriter> FileWriterPtr;

/// File writer.

class Y_API FileWriter: public Object
{
public:

	/// Create a duplicate FileWriter.
	/// \return Duplicate FileWriter or NULL.

	virtual FileWriterPtr dup(Allocator *allocator = HeapAllocator::instance()) = 0;

	/// Write \a size bytes from the \a buffer.
	/// \return Number of bytes written or 0 on failure.

	virtual size_t write(const void *buffer, size_t size) throw() = 0;

	/// Write the \a buffer to the file.

	template <typename T>
	bool write(const T &buffer) throw()
	{
		return write(&buffer, sizeof buffer) == sizeof buffer;
	}

	/// Flush all written data to the storage media.

	virtual bool flush() throw() = 0;

	/// Return the current file offset.

	virtual UOffset offset() throw() = 0;

	/// Move the file offset to the specified position.

	virtual bool seek(Offset offset, Whence whence = Absolute) throw() = 0;

	/// Return the current file size.

	virtual UOffset size() throw() = 0;

	/// Change the size of the file.

	virtual bool resize(UOffset size) throw() = 0;

	/// Truncate the file past the current pointer.

	virtual bool truncate() throw() = 0;

public:

	/// Create a file writer.

	static FileWriterPtr open(const StaticString &name, Allocator *allocator = HeapAllocator::instance());

protected:

	FileWriter(size_t counter = 1) throw()
		: Object(counter)
	{
	}
};

/// Utility class for data transfer between files.
/// \tparam buffer_size Size of a buffer to use.

template <size_t buffer_size>
class FileTransfer
{
public:

	/// Performs the transfer till the \a reader 's end, then truncate the \a writer.

	FileTransfer(FileWriter *writer, FileReader *reader) throw()
	{
		size_t size;

		while ((size = reader->read(_buffer, buffer_size)))
		{
			writer->write(_buffer, size);
		}
		writer->truncate();
	}

private:

	uint8_t _buffer[buffer_size];
};

} // namespace Yttrium

#endif // __Y_FILES_HPP
