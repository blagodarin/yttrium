/// \file
/// \brief

#ifndef _include_yttrium_io_writer_h_
#define _include_yttrium_io_writer_h_

#include <yttrium/global.h>

#include <memory>

namespace Yttrium
{
	class Buffer;
	class File;

	///
	class Y_API Writer
	{
	public:
		/// Creates a Writer for a Buffer.
		explicit Writer(Buffer&);

		/// Creates a Writer for a File.
		explicit Writer(File&);

		///
		void reserve(size_t);

		///
		size_t write(const void*, size_t);

		///
		template <typename T>
		bool write(const T& data) { return write(&data, sizeof data) == sizeof data; }

		///
		uint64_t write_all(const File&);

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
