#ifndef _src_io_writer_h_
#define _src_io_writer_h_

#include <yttrium/io/writer.h>

namespace Yttrium
{
	class FilePrivate;

	class WriterPrivate
	{
	public:
		WriterPrivate(uint64_t size) : _size(size), _offset(size) {}
		virtual ~WriterPrivate() = default;

		virtual void reserve(uint64_t) = 0;
		virtual void resize(uint64_t) = 0;
		virtual size_t write_at(uint64_t, const void*, size_t) = 0;

	private:
		uint64_t _size = 0;
		uint64_t _offset = 0;

		friend Writer;
	};

	class BufferWriter : public WriterPrivate
	{
	public:
		BufferWriter(Buffer&);

		void reserve(uint64_t) override;
		void resize(uint64_t) override;
		size_t write_at(uint64_t, const void*, size_t) override;

	private:
		Buffer& _buffer;
	};

	class FileWriter : public WriterPrivate
	{
	public:
		FileWriter(std::shared_ptr<FilePrivate>&&);

		void reserve(uint64_t) override {}
		void resize(uint64_t) override;
		size_t write_at(uint64_t, const void*, size_t) override;

	private:
		const std::shared_ptr<FilePrivate> _file;
	};
}

#endif
