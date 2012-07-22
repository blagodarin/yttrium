/// \file
/// \brief %File facilities.

#ifndef __Y_FILE_H
#define __Y_FILE_H

#include <Yttrium/allocator.h>
#include <Yttrium/static_string.h>
#include <Yttrium/types.h>

namespace Yttrium
{

class PackageReader;
class PackageWriter;
class String;

/// File reader.

class Y_API File
{
	friend class PackageReader;
	friend class PackageWriter;

public:

	/// %File access mode.

	typedef uint_fast8_t Mode;

	enum ModeFlag: Mode
	{
		Read      = 1 << 0,       ///<
		Write     = 1 << 1,       ///<
		ReadWrite = Read | Write, ///<
		Pipe      = 1 << 2,       ///< The file is a pipe (has no offset and size).
	};

	/// Valid "whences" (origins) for the seek function.

	enum Whence
	{
		Absolute, ///< Offset from the beginning of file.
		Relative, ///< Offset from the current position.
		Reverse,  ///< Offset from the end of file (added).
	};

public:

	///

	File() noexcept
		: _private(nullptr)
		, _offset(0)
		, _size(0)
		, _base(0)
	{
	}

	///

	File(const StaticString &name, Mode mode, Allocator *allocator = DefaultAllocator) noexcept
		//: File() // TODO: Uncomment.
		: _private(nullptr)
		, _offset(0)
		, _size(0)
		, _base(0)
	{
		open(name, mode, allocator);
	}

	///

	explicit File(const StaticString &name, Allocator *allocator = DefaultAllocator) noexcept
		//: File() // TODO: Uncomment.
		: _private(nullptr)
		, _offset(0)
		, _size(0)
		, _base(0)
	{
		open(name, allocator);
	}

	///

	File(const File &file) noexcept;

	///

	~File() noexcept
	{
		close();
	}

public:

	///

	void close() noexcept;

	/// Flush all written data to the storage media.

	bool flush() noexcept;

	///

	bool is_opened() const noexcept;

	/// Return the current file offset.
	/// \return Current offset.

	UOffset offset() const noexcept
	{
		return _offset;
	}

	///

	bool open(const StaticString &name, Mode mode, Allocator *allocator = DefaultAllocator) noexcept;

	/// Open a file for reading using the package manager if possible.

	bool open(const StaticString &name, Allocator *allocator = DefaultAllocator) noexcept;

	/// Read the specified amount of bytes into the buffer.
	/// \return Number of bytes read or 0 on failure.

	size_t read(void *buffer, size_t size) noexcept;

	/// Read the buffer from the file.
	/// \param buffer Buffer to read.
	/// \return \c true on success.

	template <typename T>
	bool read(T *buffer) noexcept
	{
		return (read(buffer, sizeof(T)) == sizeof(T));
	}

	///

	bool read_all(String *string) noexcept;

	///

	bool read_line(String *string) noexcept;

	/// Change the size of the file.

	bool resize(UOffset size) noexcept;

	/// Set the file offset to the specified position.

	bool seek(UOffset offset, Whence whence = Absolute) noexcept;

	/// Return the file size.
	/// \return File size.

	UOffset size() const noexcept;

	///

	bool skip(UOffset size) noexcept
	{
		return seek(size, Relative);
	}

	/// Truncate the file past the current pointer.

	bool truncate() noexcept
	{
		return resize(_offset);
	}

	/// Write \a size bytes from the \a buffer.
	/// \return Number of bytes written or 0 on failure.

	size_t write(const void *buffer, size_t size) noexcept;

	/// Write the \a buffer to the file.

	template <typename T>
	bool write(const T &buffer) noexcept
	{
		return write(&buffer, sizeof buffer) == sizeof buffer;
	}

public:

	///

	File &operator =(const File &file) noexcept;

public:

	class Private;

protected:

	explicit File(Private *private_) noexcept
		: _private(private_)
		, _offset(0)
		, _size(0)
		, _base(0)
	{
	}

	Y_PRIVATE File(Private *private_, UOffset base, UOffset size) noexcept;

private:

	Private *_private;
	UOffset  _offset;
	UOffset  _size;
	UOffset  _base;
};

/// Utility class for data transfer between files.
/// \tparam buffer_size Size of a buffer to use.

template <size_t buffer_size>
class FileTransfer
{
public:

	/// Performs the transfer till the \a reader 's end, then truncate the \a writer.

	FileTransfer(File *writer, File *reader) noexcept
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

#endif // __Y_FILE_H
