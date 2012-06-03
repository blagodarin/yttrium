#ifndef __PACKAGE_YPQ_H
#define __PACKAGE_YPQ_H

#include <map>    // map
#include <vector> // vector

#include <Yttrium/string.h>

#include "package.h"

namespace Yttrium
{

class YpqReader: public PackageReader::Private
{
public:

	YpqReader(Allocator *allocator)
		: PackageReader::Private(allocator)
	{
	}

public:

	virtual bool open();

	virtual PackedFile open_file(const StaticString &name);

private:

	typedef std::map<String, UOffset> Index;

private:

	Index _index;
};

class YpqWriter: public PackageWriter::Private
{
public:

	YpqWriter(Allocator *allocator)
		: PackageWriter::Private(allocator)
		, _last_offset(0)
	{
	}

	virtual ~YpqWriter();

public:

	virtual PackedFile open_file(const StaticString &name);

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
