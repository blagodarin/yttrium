#ifndef _src_io_reader_h_
#define _src_io_reader_h_

#include <yttrium/io/file.h>
#include <yttrium/io/reader.h>

namespace Yttrium
{
	class ReaderPrivate
	{
	public:
		ReaderPrivate(uint64_t size) : _size(size) {}
		virtual ~ReaderPrivate() = default;

	protected:
		virtual size_t read(void*, size_t, uint64_t) const = 0;

	private:
		const uint64_t _size;
		uint64_t _offset = 0;

		friend Reader;
	};

	class ReaderBuffer : public ReaderPrivate
	{
	public:
		ReaderBuffer(const std::shared_ptr<const Buffer>&);

	private:
		size_t read(void*, size_t, uint64_t) const override;

	private:
		const std::shared_ptr<const Buffer> _buffer;
	};

	class ReaderFile : public ReaderPrivate
	{
	public:
		ReaderFile(File&&);

	private:
		size_t read(void*, size_t, uint64_t) const override;

	private:
		mutable File _file; // TODO: Drop 'mutable'.
	};
}

#endif
