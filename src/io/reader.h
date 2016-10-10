#ifndef _src_io_reader_h_
#define _src_io_reader_h_

#include <yttrium/io/reader.h>

namespace Yttrium
{
	class FilePrivate;

	class ReaderPrivate
	{
	public:
		ReaderPrivate(uint64_t size) : _size(size) {}
		virtual ~ReaderPrivate() = default;

		virtual size_t read_at(uint64_t, void*, size_t) const = 0;

	private:
		const uint64_t _size;
		uint64_t _offset = 0;

		friend Reader;
	};

	class BufferReader : public ReaderPrivate
	{
	public:
		BufferReader(const std::shared_ptr<const Buffer>&);
		BufferReader(Buffer&&);

		size_t read_at(uint64_t, void*, size_t) const override;

	private:
		const std::shared_ptr<const Buffer> _buffer;
	};

	class FileReader : public ReaderPrivate
	{
	public:
		FileReader(std::shared_ptr<const FilePrivate>&&);

		size_t read_at(uint64_t, void*, size_t) const override;

	private:
		const std::shared_ptr<const FilePrivate> _file;
	};

	class ReaderReader : public ReaderPrivate
	{
	public:
		ReaderReader(const std::shared_ptr<const ReaderPrivate>&, uint64_t base, uint64_t size);

		size_t read_at(uint64_t, void*, size_t) const override;

	private:
		const std::shared_ptr<const ReaderPrivate> _reader;
		const uint64_t _base;
	};
}

#endif
