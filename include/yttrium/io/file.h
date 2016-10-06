/// \file
/// \brief %File facilities.

#ifndef _include_yttrium_io_file_h_
#define _include_yttrium_io_file_h_

#include <yttrium/memory/shared_ptr.h>

namespace Yttrium
{
	class Buffer;
	class StaticString;
	class String;

	/// %File access class.
	class Y_API File
	{
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

		///
		File(const StaticString& path, unsigned mode, Allocator& = *DefaultAllocator);

		/// Open a file for reading using the resource manager if possible.
		File(const StaticString& path, Allocator& = *DefaultAllocator);

		///
		File(Special, Allocator& = *DefaultAllocator);

		///
		File(Buffer&&, Allocator& = *DefaultAllocator);

		/// Copies all available contents from another file.
		/// Returns the number of bytes written.
		uint64_t copy_all_from(File&);

		/// Flushes buffered data to the storage medium.
		bool flush();

		///
		StaticString name() const;

		/// Returns the current offset in the file.
		uint64_t offset() const;

		/// Read a block of data from the file.
		/// \param buffer The buffer to read into.
		/// \param size Buffer size.
		/// \return Number of bytes read or 0 on failure.
		size_t read(void* buffer, size_t size);

		/// Read the buffer from the file.
		/// \param buffer Buffer to read.
		/// \return \c true on success.
		template <typename T>
		bool read(T* buffer) { return read(buffer, sizeof(T)) == sizeof(T); }

		///
		bool read_all(Buffer* buffer);

		///
		bool read_all(String* string);

		///
		bool read_line(String& string);

		/// Resizes the file.
		bool resize(uint64_t size);

		/// Adjusts the current offset in the file.
		bool seek(uint64_t offset, Whence = Absolute);

		/// Returns the size of the file.
		uint64_t size() const;

		///
		bool skip(uint64_t size) { return seek(size, Relative); }

		///
		String to_string();

		/// Truncates the file past the current offset.
		bool truncate() { return resize(offset()); }

		/// Write a buffer to the file.
		/// \param buffer The buffer to write.
		/// \param size Buffer size.
		/// \return Number of bytes written or 0 on failure.
		size_t write(const void* buffer, size_t size);

		/// Write the \a buffer to the file.
		template <typename T>
		bool write(const T& buffer) { return write(&buffer, sizeof buffer) == sizeof buffer; }

		///
		bool write_all(const Buffer&);

		/// Reads the specified file to the buffer.
		/// The buffer is guaranteed to be implicitly zero-terminated.
		static Buffer read_to_buffer(const StaticString& path, Allocator& = *DefaultAllocator);

	private:
		SharedPtr<class FilePrivate> _private;
		friend FilePrivate;

	public:
		File() = default;
		File(const File&) = delete;
		File(File&&) noexcept;
		~File();
		File& operator=(const File&) = delete;
		File& operator=(File&&) noexcept;
		explicit operator bool() const { return static_cast<bool>(_private); }
	};
}

#endif
