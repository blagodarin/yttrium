#ifndef _src_storage_reader_h_
#define _src_storage_reader_h_

#include <yttrium/storage/reader.h>
#include <yttrium/string.h>
#include <yttrium/tiny_string_map.h>

namespace Yttrium
{
	class ReaderPrivate
	{
	public:
		ReaderPrivate(uint64_t size) : _size(size) {}
		ReaderPrivate(uint64_t size, const String& name) : _name(name), _size(size) {}
		virtual ~ReaderPrivate() = default;

		virtual size_t read_at(uint64_t, void*, size_t) const = 0;

		static const void* data(const Reader& reader) noexcept { return reader._private ? reader._private->data() : nullptr; }

	protected:
		virtual const void* data() const noexcept { return nullptr; }

	private:
		const String _name{ &NoAllocator };
		const uint64_t _size;
		uint64_t _offset = 0;
		TinyStringMap _properties;

		friend Reader;
	};

	class BufferReader : public ReaderPrivate
	{
	public:
		BufferReader(Buffer&&);
		BufferReader(const std::shared_ptr<const Buffer>&, const String&);

		size_t read_at(uint64_t, void*, size_t) const override;

	private:
		const void* data() const noexcept override;

	private:
		const std::shared_ptr<const Buffer> _buffer;
	};

	class ReaderReader : public ReaderPrivate
	{
	public:
		ReaderReader(const std::shared_ptr<const ReaderPrivate>&, uint64_t base, uint64_t size);
		ReaderReader(const std::shared_ptr<const ReaderPrivate>&, uint64_t base, uint64_t size, const String& name);

		size_t read_at(uint64_t, void*, size_t) const override;

	private:
		const std::shared_ptr<const ReaderPrivate> _reader;
		const uint64_t _base;
	};

	class SpanReader : public ReaderPrivate
	{
	public:
		SpanReader(const void*, size_t);

		size_t read_at(uint64_t, void*, size_t) const override;

	private:
		const void* data() const noexcept override;

	private:
		const void* const _data;
	};
}

#endif
