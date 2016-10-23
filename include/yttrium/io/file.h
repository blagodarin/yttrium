/// \file
/// \brief

#ifndef _include_yttrium_io_file_h_
#define _include_yttrium_io_file_h_

#include <yttrium/global.h>

#include <memory>

namespace Yttrium
{
	class Buffer;
	class Reader;
	class StaticString;
	class Writer;

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
		File(const StaticString& path, unsigned mode);

		///
		File(Special);

		/// Flushes buffered data to the storage medium.
		bool flush();

		///
		StaticString name() const;

		/// Returns the current offset.
		uint64_t offset() const;

		/// Reads data from the file.
		size_t read(void*, size_t);

		/// Reads data from the file.
		template <typename T>
		bool read(T& data) { return read(data, sizeof data) == sizeof data; }

		/// Resizes the file.
		bool resize(uint64_t);

		/// Adjusts the current offset.
		bool seek(uint64_t, Whence = Absolute);

		/// Returns the size of the file.
		uint64_t size() const;

		/// Truncates the file past the current offset.
		bool truncate() { return resize(offset()); }

		/// Writes data to the file.
		size_t write(const void*, size_t);

		/// Writes data to the file.
		template <typename T>
		bool write(const T& data) { return write(&data, sizeof data) == sizeof data; }

		/// Writes data at the specified offset.
		size_t write_at(uint64_t offset, const void* data, size_t size);

		/// Writes data at the specified offset.
		template <typename T>
		bool write_at(uint64_t offset, const T& data) { return write_at(offset, &data, sizeof data) == sizeof data; }

	private:
		std::shared_ptr<class FilePrivate> _private;
		friend FilePrivate;
		friend Reader;
		friend Writer;

	public:
		File() noexcept;
		File(const File&) = delete;
		File(File&&) noexcept;
		~File();
		File& operator=(const File&) = delete;
		File& operator=(File&&) noexcept;
		explicit operator bool() const { return static_cast<bool>(_private); }
	};
}

#endif
