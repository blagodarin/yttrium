#ifndef _src_storage_formats_ypq_h_
#define _src_storage_formats_ypq_h_

#include <yttrium/memory/buffer.h>
#include <yttrium/std/map.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/writer.h>
#include "../package.h"

#include <vector>

namespace Yttrium
{
	class YpqReader : public PackageReader
	{
	public:
		YpqReader(String&&, Reader&&, Allocator&);
		~YpqReader() override;

		Reader open(const StaticString&) const override;

	private:
		struct Entry;

		const String _name;
		const Reader _reader;
		Buffer _index_buffer;
		StdMap<StaticString, Entry> _entries;
		std::vector<std::pair<StaticString, StaticString>> _properties; // TODO: Map TinyStringMap into the index buffer.
	};

	class YpqWriter : public PackageWriter
	{
	public:
		YpqWriter(Writer&&);
		~YpqWriter() override;

		bool add(const StaticString&, std::map<std::string, std::string>&&) override;
		bool commit() override;

	private:
		struct Entry;

		Writer _writer;
		std::vector<Entry> _entries;
		bool _committed = false;
		bool _finished = false;
	};
}

#endif
