#ifndef __PACKAGE_YPQ_H
#define __PACKAGE_YPQ_H

#include <map> // map

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

	typedef std::map<String, uint64_t> Index;

private:

	Index _index;
};

class YpqWriter: public PackageWriter::Private
{
public:

	YpqWriter(Allocator *allocator)
		: PackageWriter::Private(allocator)
	{
	}

	virtual ~YpqWriter();

public:

	virtual PackedFile open_file(const StaticString &name);
};

} // namespace Yttrium

#endif // __PACKAGE_YPQ_H
