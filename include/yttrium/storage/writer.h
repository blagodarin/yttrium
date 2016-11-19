/// \file
/// \brief

#ifndef _include_yttrium_storage_writer_h_
#define _include_yttrium_storage_writer_h_

#include <yttrium/global.h>

#include <memory>

namespace Yttrium
{
	class Buffer;
	class Reader;
	class StaticString;

	///
	class Y_API Writer
	{
	public:
		/// Creates a Writer for a Buffer.
		explicit Writer(Buffer&);

		/// Creates a Writer for the specified file.
		explicit Writer(const StaticString& path);

		/// Returns the current offset.
		uint64_t offset() const;

		///
		void reserve(uint64_t);

		///
		void resize(uint64_t);

		/// Sets the current offset.
		bool seek(uint64_t);

		/// Returns the size of the target data.
		uint64_t size() const;

		///
		void unlink();

		/// Writes data to the target.
		size_t write(const void*, size_t);

		/// Writes data to the target.
		template <typename T>
		bool write(const T& data) { return write(&data, sizeof data) == sizeof data; }

		/// Writes data to the target.
		bool write_all(const void* data, size_t size) { return write(data, size) == size; }

		///
		bool write_all(const Buffer&);

		///
		bool write_all(const Reader&);

		///
		bool write_all(const StaticString&);

		/// Writes data at the specified offset.
		size_t write_at(uint64_t offset, const void* data, size_t size);

		/// Writes data at the specified offset.
		template <typename T>
		bool write_at(uint64_t offset, const T& data) { return write_at(offset, &data, sizeof data) == sizeof data; }

	private:
		std::unique_ptr<class WriterPrivate> _private;
		friend WriterPrivate;

	public:
		Writer() noexcept;
		Writer(Writer&&) noexcept;
		~Writer();
		Writer& operator=(Writer&&) noexcept;
		explicit operator bool() const { return static_cast<bool>(_private); }
	};
}

#endif
