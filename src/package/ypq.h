#ifndef _src_package_ypq_h_
#define _src_package_ypq_h_

#include <yttrium/std/map.h>
#include <yttrium/std/vector.h>
#include "package.h"

namespace Yttrium
{
	class YpqReader : public PackageReaderImpl
	{
	public:

		YpqReader(File&& file, Allocator& allocator);

		PackedFile do_open_file(const StaticString& name) override;

	private:

		StdMap<String, uint64_t> _index;
	};

	class YpqWriter : public PackageWriterImpl
	{
	public:

		YpqWriter(File&& file, Allocator& allocator) : PackageWriterImpl(std::move(file), allocator), _entries(allocator) {}
		~YpqWriter() override;

		PackedFile do_open_file(const StaticString& name) override;

	private:

		struct Entry
		{
			uint64_t offset;
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

		uint64_t _last_offset = 0;
		StdVector<Entry> _entries;
	};
}

#endif
