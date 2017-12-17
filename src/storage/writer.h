#ifndef _src_storage_writer_h_
#define _src_storage_writer_h_

#include <yttrium/storage/writer.h>

namespace Yttrium
{
	class WriterPrivate
	{
	public:
		WriterPrivate() noexcept = default;
		virtual ~WriterPrivate() noexcept = default;

		virtual void reserve(uint64_t) = 0;
		virtual void resize(uint64_t) = 0;
		virtual void unlink() = 0;
		virtual size_t write_at(uint64_t, const void*, size_t) = 0;

	private:
		uint64_t _size = 0;
		uint64_t _offset = 0;

		friend Writer;
	};

	class BufferWriter final : public WriterPrivate
	{
	public:
		explicit BufferWriter(Buffer&) noexcept;

		void reserve(uint64_t) override;
		void resize(uint64_t) override;
		void unlink() override {}
		size_t write_at(uint64_t, const void*, size_t) override;

	private:
		Buffer& _buffer;
	};
}

#endif
