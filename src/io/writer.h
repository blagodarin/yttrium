#ifndef _src_io_writer_h_
#define _src_io_writer_h_

#include <yttrium/io/writer.h>

namespace Yttrium
{
	class WriterPrivate
	{
	public:
		virtual ~WriterPrivate() = default;

		virtual void reserve(size_t) = 0;
		virtual size_t write(const void*, size_t) = 0;
	};

	class BufferWriter : public WriterPrivate
	{
	public:
		BufferWriter(Buffer&);

		void reserve(size_t) override;
		size_t write(const void*, size_t) override;

	private:
		Buffer& _buffer;
		size_t _offset = 0;
	};

	class FileWriter : public WriterPrivate
	{
	public:
		FileWriter(File& file) : _file(file) {}

		void reserve(size_t) override {}
		size_t write(const void*, size_t) override;

	private:
		File& _file;
	};
}

#endif
