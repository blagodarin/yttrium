/// \file
/// \brief

#ifndef _include_yttrium_io_reader_h_
#define _include_yttrium_io_reader_h_

#include <yttrium/memory/global.h>

#include <memory>

namespace Yttrium
{
	class Buffer;
	class File;
	class StaticString;
	class String;

	///
	class Y_API Reader
	{
	public:
		/// Creates a Reader for a Buffer.
		Reader(const std::shared_ptr<const Buffer>&);

		/// Creates a reader for the specified file.
		Reader(File&&);

		/// Creates a reader for the specified file using the resource manager if possible.
		Reader(const StaticString& path, Allocator& = *DefaultAllocator);

		/// Returns the current offset.
		uint64_t offset() const;

		/// Reads a block of data from the source.
		size_t read(void* buffer, size_t size);

		/// Reads the buffer from the source.
		template <typename T>
		bool read(T& buffer) { return read(&buffer, sizeof buffer) == sizeof buffer; }

		/// Reads the entire source into the buffer.
		bool read_all(Buffer&) const;

		/// Reads the entire source into the string.
		bool read_all(String&) const;

		/// Reads a block of data from the source at the specified offset.
		size_t read_at(uint64_t offset, void* buffer, size_t size) const;

		/// Reads the buffer from the source at the specified offset.
		template <typename T>
		bool read_at(uint64_t offset, T& buffer) const { return read_at(offset, &buffer, sizeof buffer) == sizeof buffer; }

		/// Sets the current offset.
		bool seek(uint64_t offset);

		/// Returns the size of the source.
		uint64_t size() const;

		/// Advances the current offset by the specified number of bytes.
		bool skip(uint64_t size);

	private:
		std::unique_ptr<class ReaderPrivate> _private;
		friend ReaderPrivate;

	public:
		Reader() noexcept;
		Reader(const Reader&) = delete;
		Reader(Reader&&) noexcept;
		~Reader();
		Reader& operator=(const Reader&) = delete;
		Reader& operator=(Reader&&) noexcept;
		explicit operator bool() const { return static_cast<bool>(_private); }
	};
}

#endif
