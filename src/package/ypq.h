#ifndef __PACKAGE_YPQ_H
#define __PACKAGE_YPQ_H

#include <yttrium/string.h>

#include "package.h"

#include <map>
#include <vector>

namespace Yttrium
{

class YpqReader
	: public PackageReader::Private
{
public:

	YpqReader(Allocator *allocator)
		: PackageReader::Private(allocator)
	{
	}

public:

	bool open() override;

	PackedFile open_file(const StaticString &name) override;

private:

	typedef std::map<String, UOffset> Index;

private:

	Index _index;
};

class YpqWriter
	: public PackageWriter::Private
{
public:

	YpqWriter(Allocator *allocator)
		: PackageWriter::Private(allocator)
		, _last_offset(0)
	{
	}

	~YpqWriter() override;

public:

	PackedFile open_file(const StaticString &name) override;

private:

	struct Entry
	{
		UOffset offset;
		String  name;

		Entry(UOffset offset, const String &name)
			: offset(offset)
			, name(name)
		{
		}
	};

	typedef std::vector<Entry> Entries;

private:

	void flush_file();

private:

	UOffset _last_offset;
	Entries _entries;
};

} // namespace Yttrium

#endif // __PACKAGE_YPQ_H
