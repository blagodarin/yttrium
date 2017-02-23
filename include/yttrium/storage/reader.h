/// \file
/// \brief

#ifndef _include_yttrium_storage_reader_h_
#define _include_yttrium_storage_reader_h_

#include <yttrium/memory/global.h>

#include <memory>
#include <string>

namespace Yttrium
{
	class Buffer;
	class StaticString;
	class TemporaryFile;

	///
	class Y_API Reader
	{
	public:
		/// Creates a Reader for a custom data range. The data must stay valid for the lifetime of the Reader.
		Reader(const void* data, size_t size);

		/// Creates a Reader for a Buffer.
		explicit Reader(Buffer&&);

		/// Creates a Reader for the specified file.
		explicit Reader(const std::string& path);

		/// Creates a Reader for a temporary file.
		explicit Reader(const TemporaryFile&);

		/// Creates a Reader for a range of another Reader.
		Reader(const Reader&, uint64_t base, uint64_t size);

		/// Returns the name of the Reader's source.
		StaticString name() const;

		/// Returns the current offset.
		uint64_t offset() const;

		/// Retrieve a metadata property value by its name.
		StaticString property(const StaticString&) const;

		/// Reads data from the source.
		size_t read(void*, size_t);

		/// Reads data from the source.
		template <typename T>
		bool read(T& data) { return read(&data, sizeof data) == sizeof data; }

		/// Reads data from the source.
		bool read_all(void* data, size_t size) { return read(data, size) == size; }

		/// Reads the entire source data into the buffer.
		bool read_all(Buffer&) const;

		/// Reads the entire source data into the string.
		bool read_all(std::string&) const;

		/// Reads data from the source at the specified offset.
		bool read_all_at(uint64_t offset, void* data, size_t size) const { return read_at(offset, data, size) == size; }

		/// Reads data from the source at the specified offset.
		size_t read_at(uint64_t offset, void* data, size_t size) const;

		/// Reads data from the source at the specified offset.
		template <typename T>
		bool read_at(uint64_t offset, T& data) const { return read_at(offset, &data, sizeof data) == sizeof data; }

		///
		bool read_line(std::string&);

		/// Sets the current offset.
		bool seek(uint64_t offset);

		/// Sets a metadata property.
		void set_property(const StaticString& name, const StaticString& value);

		/// Returns the size of the source data.
		uint64_t size() const;

		/// Advances the current offset by the specified number of bytes.
		bool skip(uint64_t size);

		/// Reads the entire source data into a Buffer.
		Buffer to_buffer() const;

		/// Reads the entire source data into a string.
		std::string to_string() const;

	private:
		std::shared_ptr<class ReaderPrivate> _private;
		friend ReaderPrivate;

	public:
		Reader() noexcept;
		Reader(const Reader&) = delete;
		Reader(Reader&&) noexcept;
		Reader(std::shared_ptr<class ReaderPrivate>&&) noexcept;
		~Reader();
		Reader& operator=(const Reader&) = delete;
		Reader& operator=(Reader&&) noexcept;
		explicit operator bool() const { return static_cast<bool>(_private); }
	};
}

#endif
