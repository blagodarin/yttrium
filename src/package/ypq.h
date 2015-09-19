#ifndef _src_package_ypq_h_
#define _src_package_ypq_h_

#include <yttrium/string.h>

#include "package.h"

#include <map>
#include <vector>

namespace Yttrium
{
	class YpqReader: public PackageReader::Private
	{
	public:

		YpqReader(const StaticString& name, Allocator* allocator)
			: PackageReader::Private(name, allocator)
		{
		}

		bool open() override;
		PackedFile open_file(const StaticString& name) override;

	private:

		std::map<String, uint64_t> _index;
	};

	class YpqWriter: public PackageWriter::Private
	{
	public:

		YpqWriter(const StaticString& name, unsigned mode, Allocator* allocator)
			: PackageWriter::Private(name, mode, allocator)
			, _last_offset(0)
		{
		}

		~YpqWriter() override;

	public:

		PackedFile open_file(const StaticString& name) override;

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

		uint64_t _last_offset;
		std::vector<Entry> _entries;
	};
}

#endif
