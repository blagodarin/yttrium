#ifndef _src_base_writer_h_
#define _src_base_writer_h_

#include <cstddef>

namespace Yttrium
{
	class Buffer;
	class File;

	template <typename>
	class Writer;

	template <>
	class Writer<Buffer>
	{
	public:
		Writer(Buffer&) noexcept;

		void reserve(size_t size);
		bool write(const void*, size_t);

	private:
		Buffer& _buffer;
		size_t _offset;
	};

	template <>
	class Writer<File>
	{
	public:
		Writer(File& file) : _file(file) {}

		void reserve(size_t) {}
		bool write(const void*, size_t);

	private:
		File& _file;
	};
}

#endif
