#ifndef _src_io_formats_ypq_h_
#define _src_io_formats_ypq_h_

#include <yttrium/io/reader.h>
#include <yttrium/io/writer.h>
#include <yttrium/std/map.h>
#include <yttrium/std/vector.h>
#include "../package.h"

namespace Yttrium
{
	class YpqReader : public PackageReader
	{
	public:
		YpqReader(String&&, Reader&&, Allocator&);

		Reader open(const StaticString&) const override;

	private:
		const String _name;
		Reader _reader;
		Allocator& _allocator;
		StdMap<String, uint64_t> _index;
	};

	class YpqWriter : public PackageWriter
	{
	public:
		YpqWriter(Writer&& writer, Allocator& allocator) : _writer(std::move(writer)), _allocator(allocator), _entries(_allocator) {}
		~YpqWriter() override;

		bool add(const StaticString&, const Reader&) override;
		void unlink() override;

	private:

		struct Entry
		{
			uint64_t offset;
			uint32_t size = 0;
			String   name;

			Entry(uint64_t offset, const String&& name)
				: offset(offset)
				, name(std::move(name))
			{
			}
		};

	private:
		Writer _writer;
		Allocator& _allocator;
		StdVector<Entry> _entries;
	};
}

#endif
