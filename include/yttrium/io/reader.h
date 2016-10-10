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
		explicit Reader(const std::shared_ptr<const Buffer>&);

		/// Creates a Reader for a Buffer.
		explicit Reader(Buffer&&);

		/// Creates a Reader for the specified file.
		explicit Reader(const StaticString& path);

		/// Creates a Reader for a range of another Reader.
		Reader(const Reader&, uint64_t base, uint64_t size);

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

		///
		bool read_line(String& string);

		/// Sets the current offset.
		bool seek(uint64_t offset);

		/// Returns the size of the source.
		uint64_t size() const;

		/// Advances the current offset by the specified number of bytes.
		bool skip(uint64_t size);

		/// Reads the entire source into a Buffer.
		Buffer to_buffer() const;

		/// Reads the entire source into a String.
		String to_string(Allocator& = *DefaultAllocator) const;

	private:
		std::shared_ptr<class ReaderPrivate> _private;
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
