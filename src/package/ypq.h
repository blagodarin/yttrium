#ifndef __PACKAGE_YPQ_H
#define __PACKAGE_YPQ_H

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
};

class YpqWriter: public PackageWriter::Private
{
public:

	YpqWriter(Allocator *allocator)
		: PackageWriter::Private(allocator)
	{
	}
};

} // namespace Yttrium

#endif // __PACKAGE_YPQ_H
