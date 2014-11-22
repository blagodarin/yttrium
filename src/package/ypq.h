#ifndef __PACKAGE_YPQ_H
#define __PACKAGE_YPQ_H

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

	typedef std::map<String, uint64_t> Index;

private:

	Index _index;
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

		Entry(uint64_t offset, const String &name)
			: offset(offset)
			, name(name)
		{
		}
	};

	typedef std::vector<Entry> Entries;

private:

	void flush_file();

private:

	uint64_t _last_offset;
	Entries  _entries;
};

} // namespace Yttrium

#endif // __PACKAGE_YPQ_H
