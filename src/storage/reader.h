#ifndef _src_storage_reader_h_
#define _src_storage_reader_h_

#include <yttrium/storage/reader.h>
#include <yttrium/string.h>

#include <unordered_map>

namespace Yttrium
{
	class FilePrivate;

	class ReaderPrivate
	{
	public:
		ReaderPrivate(uint64_t size) : _size(size) {}
		ReaderPrivate(uint64_t size, const String& name) : _name(name), _size(size) {}
		virtual ~ReaderPrivate() = default;

		StaticString property(const StaticString&) const;
		void set_property(const StaticString&, const StaticString&);

		virtual size_t read_at(uint64_t, void*, size_t) const = 0;

	private:
		const String _name{ &NoAllocator };
		const uint64_t _size;
		uint64_t _offset = 0;
		std::unordered_map<std::string, std::string> _properties; // TODO: Use more efficient data structure.

		friend Reader;
	};

	class BufferReader : public ReaderPrivate
	{
	public:
		BufferReader(Buffer&&);
		BufferReader(const std::shared_ptr<const Buffer>&, const String&);

		size_t read_at(uint64_t, void*, size_t) const override;

		static Reader create(const std::shared_ptr<const Buffer>&);

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
		const void* const _data;
	};
}

#endif
