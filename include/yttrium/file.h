/// \file
/// \brief %File facilities.

#ifndef _include_yttrium_file_h_
#define _include_yttrium_file_h_

#include <yttrium/static_string.h>

namespace Yttrium
{
	class Buffer;
	class PackageReader;
	class PackageWriter;
	class String;

	/// %File access class.
	class Y_API File
	{
		friend PackageReader;
		friend PackageWriter;

	public:

		enum
		{
			Read      = 1 << 0,       ///<
			Write     = 1 << 1,       ///<
			ReadWrite = Read | Write, ///<
			Pipe      = 1 << 2,       ///< The file is a pipe (has no offset and size).
			Truncate  = 1 << 3,       ///< Truncate the file after opening.
		};

		///
		enum Special
		{
			Temporary, ///<
			StdErr,    ///< Standard error output stream.
		};

		/// Valid "whences" (origins) for the seek function.
		enum Whence
		{
			Absolute, ///< Offset from the beginning of file.
			Relative, ///< Offset from the current position.
			Reverse,  ///< Offset from the end of file (added).
		};

	public:

		File(): _private(nullptr), _offset(0), _size(0), _base(0) {}
		File(const File&);
		File(File&& file): _private(file._private), _offset(file._offset), _size(file._size), _base(file._base) { file._private = nullptr; }
		~File();

		File& operator=(const File&);
		File& operator=(File&&);

		explicit operator bool() const;

		///
		File(const StaticString& name, unsigned mode, Allocator* allocator = DefaultAllocator);

		/// Open a file for reading using the package manager if possible.
		explicit File(const StaticString& name, Allocator* allocator = DefaultAllocator);

		///
		explicit File(Special special, Allocator* allocator = DefaultAllocator);

	public:

		/// Flush all written data to the storage media.
		bool flush();

		///
		StaticString name() const;

		/// Return the current file offset.
		/// \return Current offset.
		uint64_t offset() const { return _offset; }

		/// Read a block of data from the file.
		/// \param buffer The buffer to read into.
		/// \param size Buffer size.
		/// \return Number of bytes read or 0 on failure.
		size_t read(void* buffer, size_t size);

		/// Read the buffer from the file.
		/// \param buffer Buffer to read.
		/// \return \c true on success.
		template <typename T>
		bool read(T* buffer)
		{
			return read(buffer, sizeof(T)) == sizeof(T);
		}

		///
		bool read_all(Buffer* buffer);

		///
		bool read_all(String* string);

		///
		bool read_line(String& string);

		/// Change the size of the file.
		bool resize(uint64_t size);

		/// Set the file offset to the specified position.
		bool seek(uint64_t offset, Whence whence = Absolute);

		/// Return the file size.
		/// \return File size.
		uint64_t size() const;

		///
		bool skip(uint64_t size) { return seek(size, Relative); }

		/// Truncate the file past the current pointer.
		bool truncate() { return resize(_offset); }

		/// Write a buffer to the file.
		/// \param buffer The buffer to write.
		/// \param size Buffer size.
		/// \return Number of bytes written or 0 on failure.
		size_t write(const void* buffer, size_t size);

		/// Write the \a buffer to the file.
		template <typename T>
		bool write(const T& buffer) { return write(&buffer, sizeof buffer) == sizeof buffer; }

	public:

		class Private;

	protected:

		explicit File(Private* private_)
			: _private(private_)
			, _offset(0)
			, _size(0)
			, _base(0)
		{
		}

		Y_PRIVATE File(Private* private_, uint64_t base, uint64_t size);

	private:

		Private* _private;
		uint64_t _offset;
		uint64_t _size;
		uint64_t _base;
	};

	/// Utility class for data transfer between files.
	/// \tparam buffer_size Size of a buffer to use.
	template <size_t buffer_size>
	class FileTransfer
	{
	public:

		/// Performs the transfer till the \a reader 's end, then truncate the \a writer.
		FileTransfer(File* writer, File* reader)
		{
			size_t size;
			while ((size = reader->read(_buffer, buffer_size)))
				writer->write(_buffer, size);
			writer->truncate();
		}

	private:

		uint8_t _buffer[buffer_size];
	};
}

#endif
