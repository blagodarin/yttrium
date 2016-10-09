#ifndef _src_io_ypq_h_
#define _src_io_ypq_h_

#include <yttrium/std/map.h>
#include <yttrium/std/vector.h>
#include "package.h"

namespace Yttrium
{
	class YpqReader : public PackageReader
	{
	public:
		YpqReader(File&&, Allocator&);

		Reader open(const StaticString&) override;

	private:
		File _file;
		Allocator& _allocator;
		StdMap<String, uint64_t> _index;
	};

	class YpqWriter : public PackageWriter
	{
	public:
		YpqWriter(File&& file, Allocator& allocator) : _file(std::move(file)), _allocator(allocator), _entries(_allocator) {}
		~YpqWriter() override;

		File open_file(const StaticString&) override;

	private:

		struct Entry
		{
			uint64_t offset;
			uint32_t size = 0;
			String   name;

			Entry(uint64_t offset, const String& name)
				: offset(offset)
				, name(name)
			{
			}
		};

	private:

		void flush_file();

	private:
		File _file;
		Allocator& _allocator;
		uint64_t _last_base = 0;
		StdVector<Entry> _entries;
	};
}

#endif
