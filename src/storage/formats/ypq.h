#ifndef _src_storage_formats_ypq_h_
#define _src_storage_formats_ypq_h_

#include <yttrium/std/map.h>
#include <yttrium/std/vector.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/writer.h>
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
		YpqWriter(Writer&&, Allocator&);
		~YpqWriter() override;

		bool add(const StaticString&, const Reader&, std::map<std::string, std::string>&&) override;
		bool commit() override;

	private:
		struct Entry;

		Writer _writer;
		Allocator& _allocator;
		StdVector<Entry> _entries;
		bool _committed = false;
	};
}

#endif
