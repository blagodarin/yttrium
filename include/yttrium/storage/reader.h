/// \file
/// \brief

#ifndef _include_yttrium_storage_reader_h_
#define _include_yttrium_storage_reader_h_

#include <yttrium/api.h>

#include <memory>
#include <string>

namespace Yttrium
{
	class Buffer;
	class Source;

	///
	class Y_CORE_API Reader
	{
	public:
		///
		explicit Reader(const Source& source) noexcept : _source{source} {}

		/// Returns the current offset.
		uint64_t offset() const noexcept { return _offset; }

		/// Reads data from the source.
		size_t read(void*, size_t);

		/// Reads data from the source.
		template <typename T>
		bool read(T& data) { return read(&data, sizeof data) == sizeof data; }

		///
		bool read_line(std::string&);

		/// Sets the current offset.
		bool seek(uint64_t offset);

		/// Returns the size of the source data.
		uint64_t size() const;

		/// Advances the current offset by the specified number of bytes.
		bool skip(uint64_t size);

	private:
		const Source& _source;
		uint64_t _offset = 0;
	};
}

#endif
